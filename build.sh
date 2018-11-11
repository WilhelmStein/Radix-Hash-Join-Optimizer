#!/bin/bash

prog=$(basename "$0")

if [ "$1" == "-h" ] || [ "$1" == "--help" ]
then
    echo "# Options:"
    echo "# -u, --unit-define      Define a macro in a test unit"
    echo "# -g, --global-define    Define a macro globally"
    echo "# -c, --compile          Compile the specified test unit    [default: everything in the test unit directory]"
    echo "# -r, --rebuild          Recompile lib or test unit         [default: do not recompile up-to-date files]"

    echo -e "\n# Usage:"
    echo "# $prog -u [MACRO]"
    echo "# $prog -g [MACRO]"
    echo "# $prog -c [name]"
    echo "# $prog -r"

    echo -e "\n# Example: $prog -r -u __BENCHMARK__ -u __QUIET__ -g __CACHE_SIZE__=32768"

    exit 0
fi

while [ ! "$#" -eq 0 ]
do
    case "$1" in
        "-u" | "--unit-define")
        shift
        dexe="$dexe -D$1"
        shift
        ;;
        "-g" | "--global-define")
        shift
        dexe="$dexe -D$1"
        dlib="$dlib -D$1"
        shift
        ;;
        "-c" | "--compile")
        shift
        fexe=$(echo -e "$1\n$fexe")
        shift
        ;;
        "-r" | "--rebuild")
        rebuild=true
        shift
        ;;
        *)
        echo "$prog: invalid syntax! \"$*\""
        echo "                           ^"
        exit 1
        ;;
    esac
done

if ([ "$rebuild" ] && [ -z "$fexe" ]) || [ ! -z "$dlib" ]
then
    make clean
fi

make "DEFINED=$dlib"

PATH_TEST="./test"
PATH_BIN="./bin"

if [ -z "$fexe" ]
then
    fexe=$(ls $PATH_TEST | tr '\n' ' ')
fi

echo "-e" "\n*** Compiling exe files ***"
echo "***"

for name in ""$fexe""
do
    if [ -z "$name" ]
    then
        continue
    fi

    if [[ "$name" =~ (\.?/?.+)/(.+) ]]
    then
        dir=${BASH_REMATCH[1]}
        file=${BASH_REMATCH[2]}

        if [ "$dir" == "$PATH_BIN" ]
        then
            name="$file"
        else
            echo "$prog: directory mismatch! \"$dir\""
            continue
        fi
    fi

    name="$PATH_BIN/${name//.*/}.exe"

    if ([ "$rebuild" ] || [ ! -z "$dexe" ]) && [ -x "$name" ]
    then
        rm -f "$name"
    fi

    make "$name" "DEFINED=$dexe"
done

echo "***"
