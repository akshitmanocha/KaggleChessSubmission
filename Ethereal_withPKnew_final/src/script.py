import numpy as np

def write_fp16(file, value):
    f16 = np.float16(value)
    bytes = f16.tobytes()
    file.write(bytes)

# Parse the original .net file and write to binary
with open('weights/pknet_224x32x2.net', 'r') as f, open('pknet_224x32x2.bin', 'wb') as out:
    lines = f.readlines()
    # Process Layer 1 (32 neurons)
    print("Processing Layer 1 (32 neurons):")
    for i, line in enumerate(lines[:32]):
        print(f"Line {i+1}: {line.strip()}")
        parts = list(map(lambda v: float(v.strip('",')), line.strip().split()[1:]))  # Skip the initial count
        for val in parts:
            write_fp16(out, val)
            
    # Process Output Layer (2 neurons)
    print("\nProcessing Output Layer (2 neurons):")
    for i, line in enumerate(lines[32:]):
        print(f"Line {i+33}: {line.strip()}")
        parts = list(map(lambda v: float(v.strip('",')), line.strip().split()[1:]))  # Skip the initial count
        for val in parts:
            write_fp16(out, val)