#!/bin/bash
#This script is used to run the command for storing output for files cross-compile.txt
#and fileresult.txt

#path of destination files
path="/home/vishalraj/AESD/Assignment-1,2/assignment-1-vishalraj3112/assignments/assignment2"

#command for generating cross-compile.txt
aarch64-none-linux-gnu-gcc -print-sysroot -v > "$path/cross-compile.txt"

#command for generating fileresult.txt
file writer > "$path/fileresult.txt"


