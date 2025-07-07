#!/bin/bash
set -eu

#Run by typing ./build.sh
#Script for compiling the code with gcc
# -o file : outputs an executable named 'file'

#Compilation
echo "Compiling Process..."

gcc ./consumer.cpp -Wall -o consumer
gcc ./producer.cpp -Wall -o producer

#Running the Executables

echo "Running..."

./consumer & ./producer
