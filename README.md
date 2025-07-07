Created by Thomas Bond

Date: March 6th 2025

Github: TB-4126

Assignment 1

# Shared Memory Space

Submission for Programming Assignment 1,
as part of Dr. Qiang Guan's CS 33211 Operating Systems course

## Project Layout

The src folder contains all the source code.
- table.hpp
- producer.cpp
- consumer.cpp

## Compile Instructions

To compile the programs run the following two commands in a Linux environment:
- $ gcc producer.cpp -o producer
- $ gcc consumer.cpp -o consumer

Then once both are compiled run the following command:
- $ ./producer & ./consumer
