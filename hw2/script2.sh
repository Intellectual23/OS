#!/bin/bash

# This script calculates the sum of nums from 1 to 10

i=0
s=0
while [ $i -lt 10 ]; do
 ((s = s + i))
 ((i++))
done
echo $s