"""
arduino_fsr is a python library for classifying the input
from a Force Sensing Resistor and returning an appropirate
vibration response.

Connections are intended to be performed against an Arduino Nano.

Vibration responses are classified as one of these states:
- OFF
- LOW
- MEDIUM
- HIGH

Sources:
- https://www.instructables.com/Pyduino-Interfacing-Arduino-with-Python-through-se/
"""
from sklearn.model_selection import train_test_split, cross_val_score
from sklearn.metrics import classification_report
from sklearn.svm import SVC
import numpy as np
import pandas as pd
import serial
import time

hardware_enabled = False
if hardware_enabled:
    arduino_port = "/dev/ttyUSB0"

    print("Connecting to arduino on {}".format(arduino_port))
    FSR = serial.Serial(port=arduino_port, baudrate=9600)
    MOTOR_PIN = 11  # Vibration Motor connected on D11
    CLASSIFIER = None

    FSR.readline()
    print("Connected!")

# Vibration Modes
HIGH = 153
MEDIUM = 120
LOW = 45
OFF = 0
MODES = [OFF, LOW, MEDIUM, HIGH]


def analog_write(pin_number, analog_value):
    global FSR
    """
    Writes the analog value (0 to 255) on pin_number
    Internally sends b'WA{pin_number}:{analog_value}' over the serial
    connection
    """
    command = ("".join(("WA", str(pin_number), ":", str(analog_value)))).encode()
    FSR.write(command)


def vibrate_motor(vibration_mode):
    """ Sends a pwm signal to the specified vibration motor pin. """
    global MOTOR_PIN, MODES
    if vibration_mode < len(MODES) and vibration_mode >= 0:
        print("vibrating {}".format(vibration_mode))
        analog_write(MOTOR_PIN, MODES[vibration_mode])


def main():

    measurements = pd.read_csv("forces-dataset.csv")

    # Using the means where min and max are removed
    X = measurements["TightMean"].to_numpy()
    X = X.reshape(-1, 1)
    Y = measurements["Class"].to_numpy()
    x_train, x_test, y_train, y_test = train_test_split(
        X, Y, test_size=0.20
    )

    svm = SVC(kernel="linear")
    svm.fit(X, Y)

    if hardware_enabled:
        while True:
            """ Get 2 samples (one each tenth of a second) and average. """
            value1 = int(FSR.readline().decode("utf-8").rstrip())
            time.sleep(0.1)
            value2 = int(FSR.readline().decode("utf-8").rstrip())
            average = (value1 + value2) / 2.0

            print("Average reading over .2 seconds: {}".format(average))

            """ Use the average for the prediction. """
            mode = svm.predict([[average]])
            vibrate_motor(mode[0])


if __name__ == "__main__":
    main()
