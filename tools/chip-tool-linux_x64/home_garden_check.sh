#!/bin/bash

echo soil sensor value
> dump.txt
./chip-tool-debug levelcontrol read current-level 1 1 >> dump.txt
SensorVal="$(grep -oP "(?<=Data = ).*(?=,)" dump.txt)"
MaxVal=100
MinVal=40

if [[($SensorVal -gt $MaxVal)]]
then
	echo "Please water your plant"
elif [[($SensorVal -lt $MaxVal) && ($SensorVal -gt $MinVal)]]
then
	echo "Soil water is normal"
elif [[($SensorVal -lt $MinVal)]]
then
	echo "too much water"
fi

echo "$SensorVal"
