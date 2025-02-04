/*
  Ethereal is a UCI chess playing engine authored by Andrew Grant.
  <https://github.com/AndyGrant/Ethereal>     <andrew@grantnet.us>

  Ethereal is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Ethereal is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// #include <assert.h>
// #include <stdbool.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>

// #include "bitboards.h"
// #include "board.h"
// #include "evaluate.h"
// #include "network.h"
// #include "thread.h"
// #include "types.h"


#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "bitboards.h"
#include "board.h"
#include "evaluate.h"
#include "network.h"
#include "thread.h"
#include "types.h"

PKNetwork PKNN;

/* Half-precision (16-bit) to single-precision (32-bit) float conversion */
static float half_to_float(uint16_t h) {
    uint32_t sign = (h >> 15) & 0x1;
    uint32_t exponent = (h >> 10) & 0x1F;
    uint32_t mantissa = h & 0x3FF;

    uint32_t f32_sign = sign << 31;
    uint32_t f32_exponent;
    uint32_t f32_mantissa;

    if (exponent == 0) { // Denormal or zero
        if (mantissa == 0) {
            return *(float*)&f32_sign; // Zero
        }
        // Denormal: normalize it
        exponent = 127 - 14; // FP16 exponent bias: 15, FP32 bias: 127
        while ((mantissa & 0x400) == 0) {
            mantissa <<= 1;
            exponent--;
        }
        mantissa &= 0x3FF;
        f32_exponent = exponent << 23;
        f32_mantissa = mantissa << 13;
    } else if (exponent == 0x1F) { // Inf/NaN
        f32_exponent = 0xFF << 23;
        f32_mantissa = mantissa ? 0x7FFFFF : 0; // NaN or Inf
    } else { // Normalized
        f32_exponent = (exponent + (127 - 15)) << 23;
        f32_mantissa = mantissa << 13;
    }

    uint32_t f32 = f32_sign | f32_exponent | f32_mantissa;
    return *(float*)&f32;
}

/* Helper to read little-endian fp16 values */
static uint16_t read_half(FILE *file) {
    unsigned char bytes[2];
    if (fread(bytes, 1, 2, file) != 2) {
        perror("Error reading half-precision float");
        exit(1);
    }
    return bytes[0] | (bytes[1] << 8);
}

void initPKNetwork() {
    FILE *file = NULL;
    const char *paths[] = {
        "/kaggle_simulations/agent/pknet_224x32x2.bin",
        "pknet_224x32x2.bin"
    };
    
    for (int i = 0; i < sizeof(paths)/sizeof(paths[0]); i++) {
        file = fopen(paths[i], "rb");
        if (file) break;
    }
    
    if (!file) {
        perror("Failed to open PKNet weights file");
        exit(1);
    }

    // Read input layer weights and biases
    for (int i = 0; i < PKNETWORK_LAYER1; i++) {
        for (int j = 0; j < PKNETWORK_INPUTS; j++) {
            uint16_t weight16 = read_half(file);
            PKNN.inputWeights[j][i] = half_to_float(weight16);
        }
        uint16_t bias16 = read_half(file);
        PKNN.inputBiases[i] = half_to_float(bias16);
    }

    // Read output layer weights and biases
    for (int i = 0; i < PKNETWORK_OUTPUTS; i++) {
        for (int j = 0; j < PKNETWORK_LAYER1; j++) {
            uint16_t weight16 = read_half(file);
            PKNN.layer1Weights[i][j] = half_to_float(weight16);
        }
        uint16_t bias16 = read_half(file);
        PKNN.layer1Biases[i] = half_to_float(bias16);
    }

    fclose(file);
}

// PKNetwork PKNN;

// static char *PKWeights[] = {
//     #include "weights/pknet_224x32x2.net"
//     ""
// };

static int computePKNetworkIndex(int colour, int piece, int sq) {
    return (64 + 48) * colour
         + (48 * (piece == KING))
         + sq - 8 * (piece == PAWN);
}


// void initPKNetwork() {

//     for (int i = 0; i < PKNETWORK_LAYER1; i++) {

//         char weights[strlen(PKWeights[i]) + 1];
//         strcpy(weights, PKWeights[i]);
//         strtok(weights, " ");

//         for (int j = 0; j < PKNETWORK_INPUTS; j++)
//             PKNN.inputWeights[j][i] = atof(strtok(NULL, " "));
//         PKNN.inputBiases[i] = atof(strtok(NULL, " "));
//     }

//     for (int i = 0; i < PKNETWORK_OUTPUTS; i++) {

//         char weights[strlen(PKWeights[i + PKNETWORK_LAYER1]) + 1];
//         strcpy(weights, PKWeights[i + PKNETWORK_LAYER1]);
//         strtok(weights, " ");

//         for (int j = 0; j < PKNETWORK_LAYER1; j++)
//             PKNN.layer1Weights[i][j] = atof(strtok(NULL, " "));
//         PKNN.layer1Biases[i] = atof(strtok(NULL, " "));
//     }
// }

int computePKNetwork(Board *board) {

    uint64_t pawns = board->pieces[PAWN];
    uint64_t kings = board->pieces[KING];
    uint64_t black = board->colours[BLACK];

    float layer1Neurons[PKNETWORK_LAYER1];
    float outputNeurons[PKNETWORK_OUTPUTS];

    // Layer 1: Compute the values in the hidden Neurons of Layer 1
    // by looping over the Kings and Pawns bitboards, and applying
    // the weight which corresponds to each piece. We break the Kings
    // into two nearly duplicate steps, in order to more efficiently
    // set and update the Layer 1 Neurons initially

    { // Do one King first so we can set the Neurons
        int sq = poplsb(&kings);
        int idx = computePKNetworkIndex(testBit(black, sq), KING, sq);
        for (int i = 0; i < PKNETWORK_LAYER1; i++)
            layer1Neurons[i] = PKNN.inputBiases[i] + PKNN.inputWeights[idx][i];
    }

    { // Do the remaining King as we would do normally
        int sq = poplsb(&kings);
        int idx = computePKNetworkIndex(testBit(black, sq), KING, sq);
        for (int i = 0; i < PKNETWORK_LAYER1; i++)
            layer1Neurons[i] += PKNN.inputWeights[idx][i];
    }

    while (pawns) {
        int sq = poplsb(&pawns);
        int idx = computePKNetworkIndex(testBit(black, sq), PAWN, sq);
        for (int i = 0; i < PKNETWORK_LAYER1; i++)
            layer1Neurons[i] += PKNN.inputWeights[idx][i];
    }

    // Layer 2: Trivially compute the Output layer. Apply a ReLU here.
    // We do not apply a ReLU in Layer 1, since we already know that all
    // of the Inputs in Layer 1 are going to be zeros or ones

    for (int i = 0; i < PKNETWORK_OUTPUTS; i++) {
        outputNeurons[i] = PKNN.layer1Biases[i];
        for (int j = 0; j < PKNETWORK_LAYER1; j++)
            if (layer1Neurons[j] >= 0.0)
                outputNeurons[i] += layer1Neurons[j] * PKNN.layer1Weights[i][j];
    }

    assert(PKNETWORK_OUTPUTS == PHASE_NB);
    return MakeScore((int) outputNeurons[MG], (int) outputNeurons[EG]);
}