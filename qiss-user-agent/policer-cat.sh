#!/bin/bash

# watch --interval=1 /home/jdg/code/qsboxta/policer-cat.sh 

for x in \
 'cat /home/jdg/code/qsboxta/policer.table ' \
 'cat /proc/qiss/tci/policer.table' \
 'cat /proc/qiss/tci/policer.count' \
; do
	echo "# ---+---> $x"
	$x
done

