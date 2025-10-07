# EEE3096S Practical 4 - Audio Signal Processing on STM32F4

## Overview

This repository contains the code and resources for Practical 4 of EEE3096S (Digital Systems) at the University of Cape Town. The practical focuses on digital signal processing and audio synthesis using the STM32F4 microcontroller, including the generation of various waveforms and audio sample playback.

## Project Features

- **Waveform Generation**:
  - Sine wave
  - Sawtooth wave
  - Triangle wave
  
- **Audio Sample Playback**:
  - Piano samples
  - Guitar samples
  - Drum samples

- **Signal Processing**:
  - 12-bit DAC output
  - Lookup table (LUT) based synthesis
  - WAV file processing

## Project Structure

- `Core/` - Main STM32 project source code
- `Task 1/` - MATLAB scripts for signal analysis
- `*.wav` - Audio sample files (piano.wav, guitar.wav, drum.wav)
- `generat_luts.py` - Python script to generate waveform LUTs
- `process_wav.py` - Python script to convert WAV files to C header files

## Requirements

### Hardware
- STM32F446RE Nucleo board (or compatible)
- Audio output circuit (DAC + amplifier + speaker)
- USB cable for programming and power

### Software
- STM32CubeIDE or Keil MDK
- Python 3.7+ (for LUT generation)
- Required Python packages:
  ```
  numpy
  scipy
  matplotlib
  ```

## Getting Started

1. **Generate Waveform LUTs**:
   ```bash
   python generat_luts.py
   ```
   This will generate C arrays for sine, sawtooth, and triangle waves.

2. **Process Audio Samples**:
   ```bash
   python process_wav.py
   ```
   This will convert WAV files into C header files for playback.

3. **Build and Flash**:
   - Open the project in your preferred IDE (STM32CubeIDE/Keil MDK)
   - Build the project
   - Flash to your STM32 board

## Usage

1. The system will start with a default waveform (sine wave)
2. Use the onboard buttons to switch between different waveforms
3. The potentiometer can be used to adjust the frequency
4. Audio samples can be triggered via external inputs

## Authors

Samson Okuthe  
Nyakallo Peete

University of Cape Town  
EEE3096S - DEmbedded Systems

---

*Last updated: October 8, 2025*
