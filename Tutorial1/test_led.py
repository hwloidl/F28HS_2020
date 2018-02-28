#!/usr/bin/python
# From: https://learn.sparkfun.com/tutorials/raspberry-gpio
# Goal: blinking an external LED connected through a breadboard
# Hardware setup: https://learn.sparkfun.com/tutorials/raspberry-gpio/hardware-setup

# External module imports
import RPi.GPIO as GPIO
import time

# Pin Definitons: start with pin 47 (on-board green LED), then use pin 26 that needs to be wired up through the GPIO pins of the RPi2
ledPin = 47 # Broadcom pin 47, ie the on-chip LED
# ledPin = 26 # Broadcom pin 26, one of the GPIO pins that can be wired to an LED

# Pin Setup:
GPIO.setmode(GPIO.BCM) # Broadcom pin-numbering scheme
GPIO.setup(ledPin, GPIO.OUT) # LED pin set as output

# Initial state for LEDs:
GPIO.output(ledPin, GPIO.LOW)

print("Here we go! Press CTRL+C to exit")
try:
    while 1:
        GPIO.output(ledPin, GPIO.HIGH)
        time.sleep(0.075)
        GPIO.output(ledPin, GPIO.LOW)
        time.sleep(0.075)
except KeyboardInterrupt: # If CTRL+C is pressed, exit cleanly:
    GPIO.cleanup() # cleanup all GPIO

