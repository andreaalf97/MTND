#!/bin/bash

for filename in ./*.txt;
do
	echo "Testing $filename";
	../../a.out < $filename;
done
