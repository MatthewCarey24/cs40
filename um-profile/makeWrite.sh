#!/bin/sh

cd um-lab
make clean
make
./writetests
cd ..
./run_tests.sh