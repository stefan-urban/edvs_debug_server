#!/bin/bash

NUMBERS="7001 7002 7003 7004 7005 7006 7007"

for number in `echo $NUMBERS`
do
	sudo ./debugcamera -c /dev/edvs_camera_debug_$number &
done

sleep 1

for number in `echo $NUMBERS`
do
	sudo ./remserial -d -p $number -s "4000000 raw" /dev/edvs_camera_debug_$number &
done

