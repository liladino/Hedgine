#!/bin/bash

# measures the test cases with different optimization levels 

cd hedgineUCI

for j in {0..3}
do
	make clean > /dev/null 2>&1
	make "opt$j" > /dev/null 2>&1
	for i in {1..10}
	do
		/usr/bin/time -f%e ./hedgine test quit | grep real 
	done 
	echo " " # to break line
done 

cd ..