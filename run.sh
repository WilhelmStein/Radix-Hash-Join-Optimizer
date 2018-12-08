#!/bin/bash

if [ "$#" -lt 5 ]
then
    echo "Usage: $(basename "$0") <UNIT> <WORKLOAD> <STDERR> <STDOUT> <FILES>"
    exit 1
fi

unit="$1";     shift
workload="$1"; shift
stderr="$1";   shift
stdout="$1";   shift

eval "./build.sh -x $unit"

files=$(for file in "$*"; do echo "$(realpath $file)"; done | rev | sort -n -k1.2 | rev)

queries=$(cat "$workload")

input=$(echo -e "$files\nDone\n$queries")

echo "$input" | "$unit" > "$stdout" 2> "$stderr"
