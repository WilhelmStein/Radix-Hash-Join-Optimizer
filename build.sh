#!/bin/bash

PATH_TEST="./test"
PATH_BIN="./bin"

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
        "-t" | "--test-dir")
        shift
        PATH_TEST="$1"
        shift
        ;;
        "-b" | "--bin-dir")
        shift
        PATH_BIN="$1"
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
