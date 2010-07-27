#!/bin/sh

./nserv &
sleep 1
./random -s -n Flo &
./random -s -n Richard &
./random -s -n Hans &
./random -s -n Randall &
