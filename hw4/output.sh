#!/bin/bash

#This script calculates the sum of even numbers from 1 to n

echo "Enter n"
read n
i=0
s=0
while [ $i -lt $n ]; do
	if(( i % 2 == 0 )); then
		((s = s + i))
	fi
	((i++))
done;

echo "The sum of even numbers from 1 to n is "$s
