import os
import math
import time
import numpy as np
import cv2
import tensorflow.keras as tf
import serial

# Configuration
DIR_PATH     = os.path.dirname(os.path.realpath(__file__))
ARDUINO_PORT = 'COM8'
BAUDRATE     = 9600
CONF_THRESH  = 90   # Confidence threshold (%)
MAX_CLASSES  = 7    # Use up to the first 7 classes

# Initialize Arduino serial port
arduino = serial.Serial(port=ARDUINO_PORT, baudrate=BAUDRATE, timeout=0.1)
time.sleep(2)  # Wait for Arduino to reset

# Load class names
labels_path = os.path.join(DIR_PATH, "keras", "labels.txt")
with open(labels_path, 'r') as f:
    classes = [line.strip().split(' ', 1)[1] for line in f]

if len(classes) < MAX_CLASSES:
    print(f"Warning: only found {len(classes)} classes, expected at least {MAX_CLASSES}.")

# Map first MAX_CLASSES classes to numbers 1–MAX_CLASSES
class_to_num = { classes[i]: i+1 for i in range(min(len(classes), MAX_CLASSES)) }

# Load Keras model
model_path = os.path.join(DIR_PATH, "keras", "keras_model.h5")
model = tf.models.load_model(model_path, compile=False)

# Open webcam
cap = cv2.VideoCapture(0)
frameW, frameH = 1280, 720
cap.set(cv2.CAP_PROP_FRAME_WIDTH,  frameW)
cap.set(cv2.CAP_PROP_FRAME_HEIGHT, frameH)
cap.set(cv2.CAP_PROP_GAIN, 0)

print(">> Starting detection, press ESC to exit.")

while True:
    # Capture a frame and preprocess
    ret, frame = cap.read()
    frame = cv2.flip(frame, 1)
    margin = (frameW - frameH) // 2
    square = frame[0:frameH, margin:margin + frameH]
    resized = cv2.resize(square, (224, 224))
    rgb     = cv2.cvtColor(resized, cv2.COLOR_BGR2RGB)
    arr     = (np.asarray(rgb).astype(np.float32) / 127.0) - 1
    data    = np.expand_dims(arr, axis=0)

    # Model inference
    preds = model.predict(data)[0] * 100  # Convert to percentage

    # Default display text
    info_text = "No detection"

    # Check for any class above threshold
    for i, conf in enumerate(preds):
        if conf > CONF_THRESH and i < MAX_CLASSES:
            cls = classes[i]
            num = class_to_num[cls]
            info_text = f"Detected: {cls} ({int(conf)}%) → Sent: {num}"
            print(info_text)                   # Print to terminal
            arduino.write(f"{num}\n".encode()) # Send pure number
            break

    # Leave space below for displaying info_text
    bordered = cv2.copyMakeBorder(
        square,
        top=0, bottom=40, left=0, right=0,
        borderType=cv2.BORDER_CONSTANT,
        value=[0, 0, 0]
    )
    cv2.putText(
        bordered, info_text,
        org=(10, frameH + 30),
        fontFace=cv2.FONT_HERSHEY_SIMPLEX,
        fontScale=0.7,
        color=(0, 255, 0),
        thickness=2
    )

    cv2.imshow("Teachable Machine Feed", bordered)
    if cv2.waitKey(1) & 0xFF == 27:  # Press ESC to exit
        break

cap.release()
cv2.destroyAllWindows()
arduino.close()
