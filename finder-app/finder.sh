#!/bin/sh
filesdir=$1 #1 argument is the file directory path
searchstr=$2 #2 argument is the string to be searched

#argument number checking
if [ "$#" -eq 0 ] || [ "$#" -eq 1 ]; then 
	echo  "Error: Wrong arguments!"
	exit 1
fi

if ! [ -d "$filesdir" ]; then #checking valid directory
	echo "Error : $filesdir is not a directory on the FileSystem!"
	exit 1 
fi

X="$(find "$filesdir" -type f | wc -l)" #https://devconnected.com/how-to-count-files-in-directory-on-linux

Y="$(grep -rnw "$filesdir" -e "$searchstr" | wc -l)" #https://stackoverflow.com/questions/16956810/how-do-i-find-all-files-containing-specific-text-on-linux
#grep -rnw "$filesdir" -e "$searchstr" #uncomment this to see string match results

#print final result
echo "The number of files are $X and the number of matching lines are $Y"

