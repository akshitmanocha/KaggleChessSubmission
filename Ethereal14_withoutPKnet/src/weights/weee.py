import torch

# Load the model with FP32 weights
model_path = '/Users/akshitmanocha/Documents/Deep Learning/KaggleChessSubmission/Ethereal-14.00/src/weights/pknet_224x32x2.net'
model = torch.load(model_path, weights_only=True)

# Convert the model to FP16 or INT8
# Option 1: Convert to FP16 (Half precision)
model_fp16 = model.half()

# Option 2: Convert to INT8 (Quantization)
# For INT8 conversion, we need to prepare the model with proper quantization methods.
# This process includes calibration if you're working with a real model.

model_fp8 = torch.quantization.quantize_dynamic(model, dtype=torch.qint8)

# Save the FP16 model
save_fp16_path = '/Users/akshitmanocha/Documents/Deep Learning/KaggleChessSubmission/Ethereal-14.00/src/weights/pknet_224x32x2_fp16.net'
torch.save(model_fp16, save_fp16_path)

# Alternatively, save the INT8 model
save_int8_path = '/Users/akshitmanocha/Documents/Deep Learning/KaggleChessSubmission/Ethereal-14.00/src/weights/pknet_224x32x2_int8.net'
torch.save(model_fp8, save_int8_path)

print("Conversion successful!")
