#!/bin/sh

./nserv &
sleep 1
./random -s -n Flo &
./random -s -n Denis &
./random -s -n Hans &
./random -s -n Randall &
