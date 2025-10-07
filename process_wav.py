import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile

# --- Configuration ---
NUM_SAMPLES = 8192  # Number of samples to take from the WAV file
MAX_VAL = 4095     # 12-bit resolution
WAV_FILES = ['piano.wav', 'guitar.wav', 'drum.wav'] # Replace with your file names
LUT_NAMES = ['Piano_LUT', 'Guitar_LUT', 'Drum_LUT']
START_SAMPLE = 10000 # Start reading from this sample to avoid initial silence
OUTPUT_DIR = 'lut_headers/' # Directory to save header files

# Helper function to save an array as a C header file
def save_c_header(filename, array_name, data):
    with open(filename, 'w') as f:
        f.write(f"#ifndef {array_name.upper()}_H\n")
        f.write(f"#define {array_name.upper()}_H\n\n")
        f.write(f"#include <stdint.h>\n\n")
        f.write(f"// Auto-generated LUT from WAV file\n")
        f.write(f"// Samples: {len(data)}\n")
        f.write(f"// 12-bit resolution (0-4095)\n\n")
        f.write(f"uint32_t {array_name}[{NUM_SAMPLES}] = {{\n")
        for i in range(0, len(data), 16):
            line = ", ".join(map(str, data[i:i+16]))
            f.write(f"    {line},\n")
        f.write("};\n\n")
        f.write(f"#endif // {array_name.upper()}_H\n")
    
    print(f"Saved {filename}")

# Create output directory if it doesn't exist
import os
os.makedirs(OUTPUT_DIR, exist_ok=True)

plt.figure(figsize=(12, 8))

for i, (filename, lut_name) in enumerate(zip(WAV_FILES, LUT_NAMES)):
    try:
        # 1. Read the WAV file
        samplerate, data = wavfile.read(filename)

        # 2. Select a slice of the audio data
        # Ensure data is single channel (mono)
        if data.ndim > 1:
            data = data[:, 0]
        audio_slice = data[START_SAMPLE : START_SAMPLE + NUM_SAMPLES]

        # 3. Normalize the data to the range [-1.0, 1.0]
        # Common WAV formats are 16-bit signed int (-32768 to 32767)
        normalized_data = audio_slice / 32768.0

        # 4. Scale and shift to the 0-4095 range
        scaled_data = np.round((normalized_data + 1) * (MAX_VAL / 2)).astype(int)

        # 5. Save as C header file
        header_filename = os.path.join(OUTPUT_DIR, f"{lut_name.lower()}.h")
        save_c_header(header_filename, lut_name, scaled_data)

        # 6. Plot for verification
        plt.subplot(len(WAV_FILES), 1, i + 1)
        plt.title(f"LUT from {filename}")
        plt.plot(scaled_data)
        plt.grid(True)

    except FileNotFoundError:
        print(f"Error: Could not find file '{filename}'. Skipping.")
    except Exception as e:
        print(f"An error occurred with {filename}: {e}")

plt.tight_layout()
plt.show()

print(f"\nAll LUT header files saved to '{OUTPUT_DIR}' directory")
print("You can include these in your C project with:")
for lut_name in LUT_NAMES:
    print(f'#include "{lut_name.lower()}.h"')