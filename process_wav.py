import numpy as np
import matplotlib.pyplot as plt
from scipy.io import wavfile

# --- Configuration ---
NUM_SAMPLES = 256  # Number of samples to take from the WAV file
MAX_VAL = 4095     # 12-bit resolution
WAV_FILES = ['piano.wav', 'guitar.wav', 'drum.wav'] # Replace with your file names
LUT_NAMES = ['Piano_LUT', 'Guitar_LUT', 'Drum_LUT']
START_SAMPLE = 10000 # Start reading from this sample to avoid initial silence

# Helper function to print an array in C format
def print_c_array(name, data):
    print(f"uint32_t {name}[{NUM_SAMPLES}] = {{")
    for i in range(0, len(data), 16):
        line = ", ".join(map(str, data[i:i+16]))
        print(f"    {line},")
    print("};")
    print("\n" + "="*50 + "\n")

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

        # 5. Print the C array
        print_c_array(lut_name, scaled_data)

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