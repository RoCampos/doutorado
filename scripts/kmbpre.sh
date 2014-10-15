#!/bin/bash

input_dir=$1
output_dir=$2

instances=`ls -vH ${input_dir}`

for i in ${instances}
do

	out=`echo $i | sed -e 's:.brite:.pre:'`

	touch ${output_dir}${out}

	#generating a preprocessing file based on a kmb algorithm
	./build/stalgo_test $input_dir${i} $output_dir$out
	cat ${output_dir}${out}

	

done
