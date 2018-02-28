#!/bin/bash
# -----------------------------------------------------------------------------
# Blink an LED connected through one of the GPIO pins (for RPi2)
#
# Adapted from these sources
# See: http://raspberrypi.znix.com/hipidocs/topic_gpiodev.htm
# and: https://www.kernel.org/doc/Documentation/gpio/gpio.txt
#
# Accessing GPIO pins through the Linux SysFS
# Usage: sudo sh ./sysfs.sh -p <PIN>
# Options: -p <PIN> ... pin on the Rpi2 to control (using BCM pin no.s)
#          -d <n>   ... delay of <n> secs (keep LED on for that amount of time)
#          -v       ... verbose messages when running the script
#          -h       ... help message
#
# -----------------------------------------------------------------------------

# These settings are for Raspberry Pi 2

# default pin is on-board ACT LED; change this by using -p
pin=47
help=0
verb=0
dry_run=0
# delay in seconds
delay=7

# process command line options
getopts "hvnd:p:" nam
while [ "$nam" != "?" ] ; do
  case $nam in
   h) help=1;;
   v) verb=1;;
   n) dry_run=1;;
   d) delay="$OPTARG";;
   p) pin="$OPTARG";;
  esac 
  getopts "hvnd:p:" nam
done

#shift $[ $OPTIND - 1 ]

# print help message
if [ $help -eq 1 ]
 then no_of_lines=`cat $0 | awk 'BEGIN { n = 0; } \
                                 /^$/ { print n; \
                                        exit; } \
                                      { n++; }'`
      echo "`head -$no_of_lines $0`"
      exit 
fi

# make this pin available through the SysFS
echo $pin > /sys/class/gpio/export

# now, set this pin to output
echo out > /sys/class/gpio/gpio${pin}/direction

if [ $verb -eq 1 ] 
  then echo "Blinking LED on pin ${pin} ..."
fi

# write a value to this pin
echo 1 > /sys/class/gpio/gpio${pin}/value

sleep ${delay}s

# write a value to this pin
echo 0 > /sys/class/gpio/gpio${pin}/value

# make this pin available through the SysFS
echo $pin > /sys/class/gpio/unexport

# tested: OK