#!/bin/bash

DEFINED=""
for arg in "$@"
do
    DEFINED="$DEFINED -D$arg"
done

if [ ! -z "$DEFINED" ]
then
    make "DEFINED=$DEFINED"
else
    make
fi

PATH_SRC="./src"
PATH_INC="./inc"
PATH_BIN="./bin"
PATH_TST="./test"

CC="g++"
CFLAGS="-Wall -Wextra -std=c++14 -g3"

echo -e "\n*** Compiling executable files ***"
echo "***"

for file in `ls test`
do
    name="${file%.cpp}"

    echo "*** Compiling test_$name"
	eval "$CC -I $PATH_INC $DEFINED $CFLAGS $PATH_TST/$file $PATH_BIN/*.o -o $PATH_BIN/test_$name"
done

echo "***"
