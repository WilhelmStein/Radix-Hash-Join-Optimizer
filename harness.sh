#!/bin/bash
yes "yes" | ./cpp-build --makefile;
if [ -z "$1" ]; then
    ./cpp-build -q;
else
    ./cpp-build -q -t=$1;
fi


yes "yes" | ./igenerate.sh workload/small/small.work input.txt workload/small/r*;

cd ./workload/small;

echo;
echo;
echo "Harness completed in (ms):";
../../bin/harness.exe small.init small.work small.result ../../bin/executioner_unit.exe;

cd ../../;

