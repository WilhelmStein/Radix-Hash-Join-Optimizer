#!/bin/bash

function confirm
{
    if [ -e "$1" ]
    then
        read -p "Are you sure you want to overwrite \"$1\": " answer
        if [[ "$answer" != [yY] ]] && [[ "$answer" != [yY][eE][sS] ]]
        then
            exit 1
        fi
    fi
}

if [ "$#" -lt 3 ]
then
    echo "Usage: $(basename "$0") <WORKLOAD> <OUT> <RELATIONS>"
    exit 1
fi

queries=$(cat "$1"); shift

out="$1"; shift

relations=$(for file in "$*"; do echo "$(realpath $file)"; done | rev | sort -n -k1.2 | rev)

input=$(echo -e "$relations\nDone\n$queries")

confirm "$out"; echo "$input" > "$out"
