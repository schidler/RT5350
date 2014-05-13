#!/bin/sh
#################
# GPIO Init
#################
echo "114" > /sys/class/gpio/export
echo "1"   > /sys/class/gpio/gpio114/active_low
echo "out" > /sys/class/gpio/gpio114/direction
sleep 1
echo "in"  > /sys/class/gpio/gpio114/direction

