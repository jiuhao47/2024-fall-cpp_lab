#!/usr/bin/env bash
# set -euxo pipefail
#
debug=$2
output=$1
rm -rf ./output

if [ ! -d ./output ]
then 
	echo "mkdir output"
	mkdir ./output
fi

for dir in `ls ./tests`
do
	if [ -d ./tests/$dir ]
	then
		echo "scan ./tests/$dir"

		if [ ! -d ./output/$dir ]
		then 
			echo "mkdir ./output/$dir"
			mkdir ./output/$dir
		fi

		for file in `ls ./tests/$dir`
		do 
			if [ -f ./tests/$dir/$file ]
			then
        if [ "$output" = "true" ]; then
            echo "./build/astbuilder -d ./tests/$dir/$file > ./output/$dir/$file.output"
            ./build/astbuilder $debug ./tests/$dir/$file > ./output/$dir/$file.output
        else
            echo "./build/astbuilder -d ./tests/$dir/$file"
            ./build/astbuilder $debug ./tests/$dir/$file
        fi
			fi
		done
	fi
done
