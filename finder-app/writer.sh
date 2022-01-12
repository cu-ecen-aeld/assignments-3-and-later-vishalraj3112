#Test string
#!/bin/bash
writefile=$1 #1 argument is the file directory path
writestr=$2 #2 argument is the string to be searched

if [ "$#" -eq 0 ] || [ "$#" -eq 1 ]; then 
	echo  "Error: Wrong arguments!"
	exit 1
fi

filename="${writefile##*/}"  # get filename
direcname="${writefile%/*}" #get only directory name

if ! [ -d "$direcname" ]; then #creating director if not present
	echo "$direcname not a directory, creating new file..."
	mkdir $direcname
fi

>$writefile #creating write_file

#check if file exists/created
if [ ! -f "$writefile" ]; then
	echo "Error: File could not be created!"
	exit 1
fi

echo "$writestr" > $writefile #writing string to file

