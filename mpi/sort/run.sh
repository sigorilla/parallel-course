#!/bin/bash

make clean
make

declare -a types=("qsort" "bsort")
declare -a max=(1000000 100000)

for i in $(seq 0 1)
do
    . loop.sh "${types[$i]}" "${max[$i]}"
done
