#!/bin/bash

# add in /etc/sudoers
# #jdg
# jdg    ALL = (ALL) NOPASSWD: /sbin/insmod
# jdg    ALL = (ALL) NOPASSWD: /sbin/rmmod 

sudo /sbin/rmmod qstcp
sleep 1
sudo /sbin/insmod /lib/modules/`uname -r`/qiss/qstcp.ko

