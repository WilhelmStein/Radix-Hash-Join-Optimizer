#!/bin/bash

if [ "$#" -lt 4 ]
then
    echo "Usage: $(basename "$0") <UNIT> <WORKLOAD> <RESULTS> <FILES>"
    exit 1
fi

unit="$1";     shift
workload="$1"; shift
results="$1";  shift

eval "./build.sh -x $unit"

files=$(for file in "$*"; do echo "$(realpath $file)"; done)

queries=$(cat "$workload")

input=$(echo -e "$files\nDone\n$queries")

echo "$input" | "$unit" &> "$results"
