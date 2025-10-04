#!/bin/sh

# printf "message: "

# read msg

msg=""

while getopts "m:" opt; do
	case $opt in
		m)
			msg="$OPTARG"
			;;
	esac
done

if [ -z "$msg" ]; then
	echo "\nUsage: commit -m \"MESSAGE\" \n"
	exit 1
fi

# echo "\nMessage ($msg) \n"

git branch
git add .
git status
git commit -m "$msg"
git push origin master
