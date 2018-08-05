#!/bin/bash

path=$1
EXE=$2

for inst in `ls ${path}/N200-100* -vh`
do

	./${EXE} --inst ${inst} --sort -

done
