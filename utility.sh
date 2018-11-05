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

        tar -zxvf $target
        exit 0
    else
        if [ ! -d "$target" ]
        then
            echo "<ERR>: No such directory!"
            exit 1
        fi

        tar -zcvf "${target////}".tar.gz "$target"
    fi
elif ([ "$1" == "-s" ] || [ "$1" == "--subset" ]) && [ "$#" -eq 3 ]
then
    shift; sup="$1"
    shift; sub="$1"

    if [ ! -f "$sup" ] || [ ! -f "$sub" ]
    then
        echo "<ERR>: No such file!"
        exit 1
    fi

    while read -r line
    do
        for word in $(echo $line | tr -d '\r' | tr -d '\n')
        do
            if [ ! "$(grep -w $word $sup)" ]
            then
                echo "<MSG>: No instance of $word in $sup"
            fi
        done
    done < "$sub"
else
    echo "# Options:"
    echo "# -t, --transfer    Transfer a file / directory over ssh"
    echo "# -z, --zip         Create / Extract a .tar.gz archive"
    echo "# -s, --subset      Check if a file is a subset of another file"

    name=$(basename "$0")

    echo -e "\n# Usage:"
    echo "# $name -t [SRC] [REMOTE] [USER] [DST] [EXCLUDE]"
    echo "# $name -z [TARGET]"
    echo "# $name -s [SUPERSET] [SUBSET]"

    echo -e "\n# Examples:"
    echo "# $name -t ./ linux01 sdi1500144 Documents"
    echo "# $name -z data/"
    echo "# $name -s todos.txt done.txt"
fi
