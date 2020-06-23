#!/bin/sh

num=$1

rm -fR ./out/data
rm -fR ./out/label
mkdir ./out/data
mkdir ./out/label

for i in `seq 1 ${num}`
do
	mkdir ./out/data/${i}
	mkdir ./out/label/${i}
	echo "mkdir ${i}"
done


