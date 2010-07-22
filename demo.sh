#!/bin/sh

cd testenv
./start.sh
cd ../clients/sdl
if [ $# -eq 2 ]
	then ./randall -i -r${1}x${2}
	else ./randall -i
fi
