import struct
import sys

def convert_weights(text_path, binary_path):
    with open(text_path, 'r') as f:
        lines = [line.strip().split()[1:] for line in f.readlines()]

    binary_data = bytearray()
    
    # Process input layer: 32 neurons, 224 weights + 1 bias each (half-precision)
    for line in lines[:32]:
        cleaned = [token.strip('",') for token in line]
        if len(cleaned) != 225:
            raise ValueError(f"Input layer line length error: {len(cleaned)}")
        for val in map(float, cleaned):
            binary_data += struct.pack('e', val)  # 'e' for 16-bit float

    # Process output layer: 2 neurons, 32 weights + 1 bias each (half-precision)
    for line in lines[32:34]:
        cleaned = [token.strip('",') for token in line]
        if len(cleaned) != 33:
            raise ValueError(f"Output layer line length error: {len(cleaned)}")
        for val in map(float, cleaned):
            binary_data += struct.pack('e', val)

    with open(binary_path, 'wb') as f:
        f.write(binary_data)

if __name__ == "__main__":
    convert_weights('./weights/pknet_224x32x2.net', './weights/pknet.bin')