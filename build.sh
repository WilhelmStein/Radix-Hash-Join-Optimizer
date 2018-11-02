#!/bin/bash

declare -A def

dtst=""
dlib=""
fexe=""

rebuild=false

while [ ! "$#" -eq 0 ]
do
    case "$1" in
        "-t" | "--test")
        shift
        dtst="$dtst -D$1"
        shift
        ;;
        "-l" | "--library")
        shift
        dlib="$dlib -D$1"
        shift
        ;;
        "-g" | "--global")
        shift
        dtst="$dtst -D$1"
        dlib="$dlib -D$1"
        shift
        ;;
        "-e" | "--executable")
        shift
        fexe="$fexe $1"
        shift
        ;;
        "-r" | "--rebuild")
        rebuild=true
        shift
        ;;
        *)
        echo "<ERR>: No such option!"
        return
        ;;
    esac
done

if [ "$rebuild" == true ]
then
    make clean
fi

make "DEFINED=$dlib"

PATH_SRC="./src"
PATH_INC="./inc"
PATH_BIN="./bin"
PATH_TST="./test"

CC="g++"
CFLAGS="-Wall -Wextra -std=c++14 -g3"

echo "-e" "\n*** Compiling executable files ***"
echo "***"

for file in `ls test`
do
    if [ -z "$fexe" ] || ([ ! -z "$fexe" ] && `echo "$fexe" | grep -q "$file"`)
    then
        name="${file%.cpp}"
        cmd="$CC -I $PATH_INC $dtst $CFLAGS $PATH_TST/$file $PATH_BIN/*.o -o $PATH_BIN/$name.exe"

        echo "$cmd"
        eval $cmd
    fi
done

echo "***"
