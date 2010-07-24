#!/bin/sh

cd testenv
./start.sh
cd ../clients/sdl
if [ $# -eq 2 ]
	then ./client -r${1}x${2}
	else ./client
fi
