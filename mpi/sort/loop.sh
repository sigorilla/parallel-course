#!/bin/bash

make clean
make

FILES=*.out

P=32
NFIRST=10000
NINC=1000
NLAST=1000000

for f in $FILES;
do
    echo "Start $f file..."
    cat /dev/null > "graph.$f.csv"
    for n in $(seq $NFIRST $NINC $NLAST)
    do
        echo -n "$n" >> "graph.$f.csv"
        for p in $(seq $P)
        do
            mpirun -np $p $f $n >> "graph.$f.csv"
        done
        echo "" >> "graph.$f.csv"
    done
done

