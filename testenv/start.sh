#!/bin/sh

./server &
sleep 1
./random -s -n r1 &
./random -s -n r2 &
./random -s -n r3 &
./random -s -n r4 &
