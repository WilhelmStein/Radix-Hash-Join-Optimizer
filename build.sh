#!/bin/bash

if [ "$1" == "-h" ] || [ "$1" == "--help" ]
then
    echo "# Options:"
    echo "# -x, --define-exe      Define a macro in a test unit"
    echo "# -g, --define-global   Define a macro globally"
    echo "# -c, --compile-exe     Compile the specified test unit     [default: everything in the test unit directory]"
    echo "# -r, --rebuild-lib     Recompile any object file           [default: do not recompile up-to-date object files]"

    name=$(basename "$0")

    echo -e "\n# Usage:"
    echo "# $name -x [MACRO]"
    echo "# $name -g [MACRO]"
    echo "# $name -c [EXE]"
    echo "# $name -r"

    echo -e "\n# Example: $name -r -x __BENCHMARK__ -x __SILENCE__ -g __CACHE_SIZE__=32768"

    exit 0
fi

while [ ! "$#" -eq 0 ]
do
    case "$1" in
        "-x" | "--define-exe")
        shift
        dtst="$dtst -D$1"
        shift
        ;;
        "-g" | "--define-global")
        shift
        dtst="$dtst -D$1"
        dlib="$dlib -D$1"
        shift
        ;;
        "-c" | "--compile-exe")
        shift
        fexe=`echo -e "$1\n$fexe"`
        shift
        ;;
        "-r" | "--rebuild-lib")
        rebuild=true
        shift
        ;;
        *)
        echo "<ERR>: No such option! \"$*\""
        echo "                        ^"
        exit 1
        ;;
    esac
done

if [ "$rebuild" == true ]
then
    make clean
fi

make "DEFINED=$dlib"

PATH_TEST="./test"
PATH_BIN="./bin"

if [ ! -d "$PATH_TEST" ] || [ ! -d "$PATH_BIN" ]
then
    echo -e "\n<ERR>: No such directory!"
    exit 2
fi

if [ -z "$fexe" ]
then
    fexe=`ls $PATH_TEST`
    fexe="${fexe//.cpp/}"
fi

echo "-e" "\n*** Compiling exe files ***"
echo "***"

while IFS= read -r name
do
    make "$PATH_BIN"/"$name".exe "DEFINED=$dtst"
done <<< "$fexe"

echo "***"
