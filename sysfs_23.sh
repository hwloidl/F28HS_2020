#!/bin/bash
#
# See: http://raspberrypi.znix.com/hipidocs/topic_gpiodev.htm
# and: https://www.kernel.org/doc/Documentation/gpio/gpio.txt
#
# Accessing GPIO pins through the Linux SysFS
# Run: sudo sh sysfs_23.sh

# the pin to control
PIN=23

# make this pin available through the SysFS
echo $PIN > /sys/class/gpio/export

# now, set this pin to output
echo out > /sys/class/gpio/gpio${PIN}/direction

# write a value to this pin
echo 1 > /sys/class/gpio/gpio${PIN}/value

sleep 7s

# write a value to this pin
echo 0 > /sys/class/gpio/gpio${PIN}/value

# make this pin available through the SysFS
echo $PIN > /sys/class/gpio/unexport

# tested: OK