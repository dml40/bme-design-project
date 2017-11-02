#!/bin/bash
export LIBRARY_PATH+=/usr/local/lib
cd build
make
cp llmtester ..
./llmtester
