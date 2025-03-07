# convert_bin_to_h.py
import os

bin_filename = 'pknet_224x32x2.bin'
header_filename = 'pknet_224x32x2.h'
array_name = 'pknet_224x32x2_data'

# Read the binary file
with open(bin_filename, 'rb') as f:
    data = f.read()

# Create the header file
with open(header_filename, 'w') as h:
    h.write('#ifndef PKNET_224X32X2_H\n')
    h.write('#define PKNET_224X32X2_H\n\n')
    
    # Optionally, you might want to include stdint.h if you use fixed-width types
    h.write('#include <stdint.h>\n\n')
    
    # Write array length as a macro (optional)
    h.write(f'#define {array_name.upper()}_LEN {len(data)}\n\n')
    
    # Write the data array as an array of unsigned char
    h.write(f'const uint8_t {array_name}[{len(data)}] = {{\n')
    
    # Format each byte as a hex literal, inserting newlines for readability.
    hex_bytes = [f'0x{byte:02X}' for byte in data]
    # For example, 12 bytes per line:
    bytes_per_line = 12
    for i in range(0, len(hex_bytes), bytes_per_line):
        line = ', '.join(hex_bytes[i:i+bytes_per_line])
        # Add a comma at the end of each line (the last one is harmless in C99 and later)
        h.write('    ' + line + ',\n')
        
    h.write('};\n\n')
    h.write('#endif // PKNET_224X32X2_H\n')
    
print(f'Header file "{header_filename}" generated successfully.')
