#!/bin/sh

cd testenv
./start.sh
cd ../clients/sdl
if [ $# -eq 3 ]
	then ./client -r${1}x${2} -n${3}
	else ./client
fi
