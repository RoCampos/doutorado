#!/bin/bash

instances_dir=$1
output_dir=$2

for i in `ls ${instances_dir}`
do
	file=`basename ${i} .brite`
	(time build/stalgo_test ${instances_dir}/${i}) &>> ${output_dir}
	#sleep 10

done
