#!/bin/bash

set -x

direction=$1
KR_PIPE="/tmp/kr_fifo"

function help()
{
	echo "$0: ['f' | 'b' | 'l' | 'r' | 's']"
}


[[ "$direction" == "" ]] && help && exit 1

if [[ $direction == 'f' ]]; then
	echo "0 0 0 1" > $KR_PIPE
	sleep 1
	echo "1 70 0 0" > $KR_PIPE
fi
if [[ $direction == 'b' ]]; then
	echo "0 0 0 1" > $KR_PIPE
	sleep 1
	echo "2 70 0 0" > $KR_PIPE
fi
if [[ $direction == 'l' ]]; then
	echo "0 0 0 1" > $KR_PIPE
	sleep 1
	echo "3 90 0 0" > $KR_PIPE
fi
if [[ $direction == 'r' ]]; then
	echo "0 0 0 1" > $KR_PIPE
	sleep 1
	echo "4 90 0 0" > $KR_PIPE
fi
if [[ $direction == 's' ]]; then
	echo "0 0 0 1" > $KR_PIPE
	sleep 1
	echo "0 0 0 0" > $KR_PIPE
fi

