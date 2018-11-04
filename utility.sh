#!/bin/bash

if ([ "$1" == "-t" ] || [ "$1" == "--transfer" ]) && [ "$#" -ge 5 ]
then
    shift; src=$(realpath "$1")
    shift; lnx="$1"
    shift; usr="$1"
    shift; dst="/home/users/$usr/$1";
    shift; exc="*.pdf .vscode .git* $@"

    echo -e "\n# source-path=\"$src\""
    echo -e "# destination-path=\"$usr@$lnx.di.uoa.gr:$dst\""
    echo -e "# ignored=\"$exc\"\n"

    exc=$(echo "$exc" | tr ' ' '\n')

    exc=$(echo -e "$exc" | sed "s/.*/--exclude='&'/" | tr '\n' ' ')

    eval "rsync -av -e ssh $exc $src $usr@$lnx.di.uoa.gr:$dst"
elif ([ "$1" == "-v" ] || [ "$1" == "--valgrind" ]) && [ "$#" -ge 2 ]
then
    shift; exe="$1"; shift

    valgrind "$@" --leak-check=full --track-origins=yes "$exe"
else
    echo "# Options:"
    echo "# -t, --transfer    Transfer a file / directory over ssh"
    echo "# -v, --valgrind    Run the specified executable under valgrind"

    name=$(basename "$0")

    echo -e "\n# Usage:"
    echo "# $name -t [SRC] [REMOTE] [USER] [DST] [EXCLUDE]"
    echo "# $name -v [EXE] [FLAGS]"

    echo -e "\n# Examples:"
    echo "# $name -t ./ linux01 sdi1500144 Documents $name"
    echo "# $name -v ./bin/unit.exe -v"
fi
