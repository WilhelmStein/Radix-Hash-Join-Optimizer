#!/bin/bash

make

PATH_SRC="./src"
PATH_INC="./inc"
PATH_BIN="./bin"
PATH_TST="./test"

CC="g++"
CFLAGS="-Wall -Wextra -std=c++14 -g3"

echo "*** Compiling test units ***"
echo "***"

i=0
for file in `ls test`
do
    name="${file%.cpp}"
    out=test_$name

    echo "Compiling $out"
	eval "$CC -I $PATH_INC $CFLAGS $PATH_TST/$file $PATH_BIN/*.o -o $PATH_BIN/$out"
    ((i++))
done

echo "***"
