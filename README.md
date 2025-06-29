# CSC_413_A3
csc413 Assignment3 RhythmFit
# CSC_413_A3 — RhythmFit

This repository contains the code and model files for **RhythmFit**, an interactive motion-to-music system built with Teachable Machine, Python, OpenCV and Arduino Mega2560.

1,## Project Overview

**RhythmFit** lets users perform one of seven predefined poses in front of a webcam. A Keras model (exported from Google’s Teachable Machine) classifies each pose in real time; the Python script sends a corresponding class number (1–7) over USB serial to an Arduino Mega2560. The Mega drives a 4×4 membrane keypad and a passive buzzer:

- **Webcam + Python**  
  - Captures a 224×224 center crop from a live video feed  
  - Runs inference with a Teachable Machine model (`keras_model.h5`)  
  - Whenever a class confidence exceeds 90%, prints and sends its index (1–7) over COM8

- **Arduino Mega2560 (RhythmFit.ino)**  
  - Listens for incoming serial digits (1–7) and plays the corresponding musical note  
  - Supports octave switching (keys A/B), 30 s “record” mode (key C), and playback (key D)  
  - Provides immediate audio feedback through a passive buzzer and visual feedback via built-in LED
 

2,## Repository Structure

CSC_413_A3/
├── keras/
│ ├── keras_model.h5 # Trained Teachable Machine model
│ └── labels.txt # Class labels (“class1”…“class7”)
├── RhythmFit.ino # Arduino sketch for keypad & buzzer control
├── TeachableMachineaArduino.py
│ # Python script: captures webcam, runs model, sends serial codes
└── README.md # Project overview and instructions


## Prerequisites

- **Hardware**  
  - Arduino Mega2560, passive buzzer on D8, 4×4 membrane keypad on D30–D37, USB cable  
  - Webcam capable of at least 640×480 resolution  

- **Software**  
  - Python 3.10+ with `tensorflow`, `opencv-python`, `numpy`, `pyserial`  
  - Arduino IDE with Mega2560 board support  

## How to Run

1. **Upload Arduino Sketch**  
   - Open `RhythmFit.ino` in the Arduino IDE  
   - Select **Tools → Board → Arduino Mega or Mega 2560** and your COM port  
   - Upload  

2. **Install Python Dependencies**  
   ```bash
   pip install tensorflow opencv-python numpy pyserial
   
3, **Run the Python Script**
  python TeachableMachineaArduino.py
  - A window titled “Camera Feed (green=224x224 crop)” will appear
  - Perform one of your seven trained poses in view of the green central box
  - When recognized (≥ 90% confidence), the script prints the pose and sends a number to the Arduino
4, Interact on Arduino
- The buzzer plays the note corresponding to the pose number (1–7)
- Use keypad keys:
- A/B to switch between low/high octave
- C to start a 30 s “record” session (beep start/end)
- D to playback the recorded sequence
