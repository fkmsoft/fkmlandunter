#!/bin/sh

cd testenv
./start.sh
cd ../clients/sdl
./randall -i -r${1}x${2} || ./randall -i
