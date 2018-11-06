#!/bin/bash

function confirm
{
    if [ -e "$1" ]
    then
        read -p "Are you sure you want to overwrite \"$1\": " answer
        if [[ "$answer" != [yY] ]] && [[ "$answer" != [yY][eE][sS] ]]
        then
            exit 2
        fi
    fi
}

if ([ "$1" == "-t" ] || [ "$1" == "--transfer" ]) && [ "$#" -ge 5 ]
then
    shift; src=$(realpath "$1")
    shift; lnx="$1"
    shift; usr="$1"
    shift; dst="/home/users/$usr/$1";
    shift; exc="*.pdf .vscode .git*"

    if [ ! "$#" -eq 0 ]
    then
        exc="$exc $@"
    fi

    echo -e "\n# source-path=\"$src\""
    echo -e "# destination-path=\"$usr@$lnx.di.uoa.gr:$dst\""
    echo -e "# ignored=\"$exc\"\n"

    exc=$(echo "$exc" | tr ' ' '\n')

    exc=$(echo -e "$exc" | sed "s/.*/--exclude='&'/" | tr '\n' ' ')

    eval "rsync -avu -e ssh $exc $src $usr@$lnx.di.uoa.gr:$dst"
elif ([ "$1" == "-z" ] || [ "$1" == "--zip" ]) && [ "$#" -eq 2 ]
then
    shift; target="$1"

    if [[ "$target" =~ .*.tar.gz$ ]]
    then
        if [ ! -f "$target" ]
        then
            echo "<ERR>: No such file!"
            exit 1
        fi

        confirm "${target//.tar.gz/}/"

        tar -zxvf $target
        exit 0
    else
        if [ ! -d "$target" ]
        then
            echo "<ERR>: No such directory!"
            exit 1
        fi

        confirm "${target////}.tar.gz"

        tar -zcvf "${target////}".tar.gz "$target"
    fi    
else
    echo "# Options:"
    echo "# -t, --transfer    Transfer a file / directory over ssh"
    echo "# -z, --zip         Create / Extract a .tar.gz archive"

    name=$(basename "$0")

    echo -e "\n# Usage:"
    echo "# $name -t [SRC] [REMOTE] [USER] [DST] [EXCLUDE]"
    echo "# $name -z [TARGET]"

    echo -e "\n# Examples:"
    echo "# $name -t . linux01 sdi1500144 Documents"
    echo "# $name -z data"
fi
