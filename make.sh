#!/bin/bash
cd build
make
cp llmtester ..
./llmtester
