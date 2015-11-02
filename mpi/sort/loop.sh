#!/bin/bash

FILE="$1.out"
OUT="graph.$1.csv"

PS=2
P=32
NFIRST=10000
NINC=1000
NLAST=$2

if [[ -x $FILE ]]
then
    echo -ne "Start \033[0;33m$1\033[0m file..."
    echo ""
    cat /dev/null > $OUT
    for n in $(seq $NFIRST $NINC $NLAST)
    do
        if [[ $(( $n % 50000 )) -eq 0 ]]
        then
            echo -ne "\tN: \033[0;36m$n\033[0m -> from $PS to $P threads"
            echo ""
        fi
        echo -n "$n" >> $OUT
        for p in $(seq $PS $P)
        do
            mpirun -np $p $FILE $n >> $OUT
        done
        echo "" >> $OUT
    done
else
    echo -ne "File \033[0;33m$FILE\033[0m is not exacutable"
    echo ""
fi
