#!/bin/bash

__CACHE_SIZE__=(4 8 16 32 64)

if [ "$#" -lt 2 ]
then
    echo "Usage: $(basename "$0") [EXE] [OUT]"
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

defs=("__MEDIUM__" "__LARGE__")
for def in "${defs[@]}"
do
    for ((pow = 4; pow <= 64; pow *= 2))
    do
        size=$((pow * 1024))
        eval "./build.sh -r -u __BENCHMARK__ -u __SILENCE__ -u $def -g __CACHE_SIZE__=$size"

        "$exe" >> "$out"
    done
done
