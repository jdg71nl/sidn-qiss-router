#!/bin/bash

echo
CMD="cp -a qstcp.ko /home/jdg/run/qstcp/"
echo "CMD> $CMD"
$CMD

echo
UNAMER=`uname -r`
echo "'uname -r' = $UNAMER"
CMD="sudo mkdir -pv /lib/modules/$UNAMER/qiss/"
echo "CMD> $CMD"
$CMD

echo
CMD="sudo cp -a qstcp.ko /lib/modules/$UNAMER/qiss/"
echo "CMD> $CMD"
$CMD

echo
CMD="md5sum /home/jdg/code/qstcp/qstcp.ko /home/jdg/run/qstcp/qstcp.ko /lib/modules/$UNAMER/qiss/qstcp.ko"
echo "CMD> $CMD"
$CMD

echo
echo "# now do:"
echo "> sudo depmod -a"
echo "> sudo vi /etc/modules"
echo "> sudo rmmod qstcp"
echo "> sudo insmod /lib/modules/`uname -r`/qiss/qstcp.ko"
echo

