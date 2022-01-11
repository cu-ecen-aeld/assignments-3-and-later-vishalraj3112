#!/bin/bash
filesdir=$1 #1 argument is the file directory path
searchstr=$2 #2 argument is the string to be searched
if [ "$#" -eq 0 ]; then 
	echo  "No arguments"
	exit 1
fi
if ! [ -d "$1" ]; then 
	echo "$1 not a directory"
	exit 1 
fi

echo "string search:"
grep -rnwc $1 -e $2

echo "file count:" 
ls $1 | wc -l

