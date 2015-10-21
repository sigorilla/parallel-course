#!/bin/sh

make
cat /dev/null > graph.csv

i=1

while [ $i -le 32 ]
do
    mpirun -np $i lab2.parallel.out >> graph.csv
    i=`expr $i + 1`
done
