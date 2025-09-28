import numpy as np
import matplotlib.pyplot as plt

# --- Configuration ---
NUM_SAMPLES = 256  # Number of samples in our LUT (NS). 256 is a good value.
MAX_VAL = 4095     # 12-bit resolution (2^12 - 1)

# Helper function to print an array in C format
def print_c_array(name, data):
    print(f"uint32_t {name}[{NUM_SAMPLES}] = {{")
    for i in range(0, len(data), 16):
        line = ", ".join(map(str, data[i:i+16]))
        print(f"    {line},")
    print("};")
    print("\n" + "="*50 + "\n")

# --- 1. Generate Sinusoid LUT ---
# Create an array of NUM_SAMPLES points from 0 to 2*pi
t = np.linspace(0, 2 * np.pi, NUM_SAMPLES, endpoint=False)
# Calculate the sine wave, which ranges from -1 to 1
sin_wave_float = np.sin(t)
# Scale and shift it to fit the 0-4095 range:
# Scale by half the max value (amplitude), then shift up by the same amount (offset)
sin_lut = np.round((sin_wave_float + 1) * (MAX_VAL / 2)).astype(int)

# --- 2. Generate Sawtooth LUT ---
# Linearly increase from 0 to MAX_VAL
saw_lut = np.linspace(0, MAX_VAL, NUM_SAMPLES, endpoint=False).astype(int)

# --- 3. Generate Triangular LUT ---
triangle_lut = np.zeros(NUM_SAMPLES, dtype=int)
# First half: ramp up from 0 to MAX_VAL
for i in range(NUM_SAMPLES // 2):
    triangle_lut[i] = int(i * (MAX_VAL / (NUM_SAMPLES / 2 - 1)))
# Second half: ramp down from MAX_VAL to 0
for i in range(NUM_SAMPLES // 2, NUM_SAMPLES):
    triangle_lut[i] = int(MAX_VAL - (i - NUM_SAMPLES/2) * (MAX_VAL / (NUM_SAMPLES / 2 - 1)))
# Ensure the peak is correct
triangle_lut[NUM_SAMPLES//2 -1] = MAX_VAL
triangle_lut[NUM_SAMPLES -1] = 0


# --- Print the C arrays to the console ---
print_c_array("Sin_LUT", sin_lut)
print_c_array("Saw_LUT", saw_lut)
print_c_array("Triangle_LUT", triangle_lut)

# --- Plotting for Verification ---
plt.figure(figsize=(12, 8))
plt.subplot(3, 1, 1)
plt.title("Sine Wave LUT")
plt.plot(sin_lut)
plt.grid(True)

plt.subplot(3, 1, 2)
plt.title("Sawtooth Wave LUT")
plt.plot(saw_lut)
plt.grid(True)

plt.subplot(3, 1, 3)
plt.title("Triangular Wave LUT")
plt.plot(triangle_lut)
plt.grid(True)

plt.tight_layout()
plt.show()