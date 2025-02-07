import numpy as np

def write_fp32(file, value):
    f32 = np.float32(value)  # Changed to float32
    bytes = f32.tobytes()
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
            write_fp32(out, val)  # Changed to write_fp32
            
    # Process Output Layer (2 neurons)
    print("\nProcessing Output Layer (2 neurons):")
    for i, line in enumerate(lines[32:]):
        print(f"Line {i+33}: {line.strip()}")
        parts = list(map(lambda v: float(v.strip('",')), line.strip().split()[1:]))  # Skip the initial count
        for val in parts:
            write_fp32(out, val)  # Changed to write_fp32