#!/bin/bash
#echo "Num threads is now" $1
export OMP_NUM_THREADS=$1
./simple $2
