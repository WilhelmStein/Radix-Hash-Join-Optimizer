#!/bin/bash

if [ "$#" -lt 2 ]
then
    echo "Usage: $(basename "$0") [EXE] [OUT] [MACROS]"
    exit 1
fi

exe="$1"; shift;
out="$1"; shift;

if [ -f "$out" ]
then
    read -p "Are you sure you want to overwrite $out: " answer
    if [[ "$answer" != [yY] ]] && [[ "$answer" != [yY][eE][sS] ]]
    then
        exit 2
    fi
fi

truncate --size=0 "$out"

macros=(__SMALL__ ""$*""); shift;

for ((pow = 4; pow <= 64; pow *= 2))
do
    eval "./build.sh -g __CACHE_SIZE__=$((pow * 1024))"
    for macro in "${macros[@]}"
    do
        eval "./build.sh -b -q -u $macro -u __CACHE_SIZE__=$((pow * 1024)) -x $exe"
        "$exe" >> "$out"
    done
done
