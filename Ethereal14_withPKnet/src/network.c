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

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitboards.h"
#include "board.h"
#include "evaluate.h"
#include "network.h"
#include "thread.h"
#include "types.h"

PKNetwork PKNN;

static int computePKNetworkIndex(int colour, int piece, int sq) {
    return (64 + 48) * colour
         + (48 * (piece == KING))
         + sq - 8 * (piece == PAWN);
}

static float half_to_float(uint16_t h) {
    // Simple half-precision to float conversion (no error checking)
    uint32_t sign = (h >> 15) & 0x1;
    uint32_t exp = (h >> 10) & 0x1f;
    uint32_t mant = h & 0x3ff;
    
    if (exp == 0x1f) {  // Inf/NaN
        exp = 0xff;
    } else if (exp == 0) {  // Denormal
        if (mant != 0) {
            exp = 0x7f - 14;
            while (!(mant & 0x400)) {
                mant <<= 1;
                exp--;
            }
            mant &= 0x3ff;
        }
    } else {
        exp += 0x7f - 0xf;
    }
    
    uint32_t f = (sign << 31) | (exp << 23) | (mant << 13);
    return *(float*)&f;
}


void initPKNetwork() {
    const char *filename = "./weights/pknet.bin";
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("Failed to open PKNet weights file");
        exit(EXIT_FAILURE);
    }

    // Calculate total 16-bit values needed
    const size_t total_halfs = PKNETWORK_LAYER1 * (PKNETWORK_INPUTS + 1) +
                              PKNETWORK_OUTPUTS * (PKNETWORK_LAYER1 + 1);

    // Read all half-precision values
    uint16_t *half_data = malloc(total_halfs * sizeof(uint16_t));
    if (!half_data) {
        perror("Failed to allocate memory for weights");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    size_t read = fread(half_data, sizeof(uint16_t), total_halfs, file);
    fclose(file);

    if (read != total_halfs) {
        fprintf(stderr, "Failed to read weights: expected %zu, got %zu\n",
                total_halfs, read);
        free(half_data);
        exit(EXIT_FAILURE);
    }

    // Convert and load input layer
    size_t offset = 0;
    for (int i = 0; i < PKNETWORK_LAYER1; ++i) {
        for (int j = 0; j < PKNETWORK_INPUTS; ++j) {
            PKNN.inputWeights[j][i] = half_to_float(half_data[offset + j]);
        }
        PKNN.inputBiases[i] = half_to_float(half_data[offset + PKNETWORK_INPUTS]);
        offset += PKNETWORK_INPUTS + 1;
    }

    // Convert and load output layer
    for (int i = 0; i < PKNETWORK_OUTPUTS; ++i) {
        for (int j = 0; j < PKNETWORK_LAYER1; ++j) {
            PKNN.layer1Weights[i][j] = half_to_float(half_data[offset + j]);
        }
        PKNN.layer1Biases[i] = half_to_float(half_data[offset + PKNETWORK_LAYER1]);
        offset += PKNETWORK_LAYER1 + 1;
    }

    free(half_data);
}

int computePKNetwork(Board *board) {

    uint64_t pawns = board->pieces[PAWN];
    uint64_t kings = board->pieces[KING];
    uint64_t black = board->colours[BLACK];

    float layer1Neurons[PKNETWORK_LAYER1];
    float outputNeurons[PKNETWORK_OUTPUTS];

    { // First King
        int sq = poplsb(&kings);
        int idx = computePKNetworkIndex(testBit(black, sq), KING, sq);
        for (int i = 0; i < PKNETWORK_LAYER1; i++)
            layer1Neurons[i] = PKNN.inputBiases[i] + PKNN.inputWeights[idx][i];
    }

    { // Second King
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

    for (int i = 0; i < PKNETWORK_OUTPUTS; i++) {
        outputNeurons[i] = PKNN.layer1Biases[i];
        for (int j = 0; j < PKNETWORK_LAYER1; j++)
            if (layer1Neurons[j] >= 0.0)
                outputNeurons[i] += layer1Neurons[j] * PKNN.layer1Weights[i][j];
    }

    assert(PKNETWORK_OUTPUTS == PHASE_NB);
    return MakeScore((int) outputNeurons[MG], (int) outputNeurons[EG]);
}