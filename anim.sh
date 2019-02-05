#!/bin/bash
rm field -rf
mkdir field
pitches="0.05 0.10 0.15 0.20 0.25 0.30 0.35 0.40 0.45 0.50 0.55 0.60 0.65 0.70 0.75 0.80 0.85 0.90 0.95 1.00"
for f in $pitches
do
    ./a.out $f > field/$f
done
for f in field/*
do
    cat $f | gnuplot -e "set terminal png;set xrange[-2:2];set yrange[-2:2];set zrange[-.2:.2];sp '-' w lines;" > $f.png
done
