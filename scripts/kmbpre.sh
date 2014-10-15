#!/bin/bash

#This file generate the files *.pre based on results from KMB algorithm

# ./scritpt ../MPP-Instances/n30/ ../<saida>

#instances de entrada
input_dir=$1

#diretórrio de saída
output_dir=$2


instances=`ls -vH ${input_dir}`

for i in ${instances}
do

	out=`echo $i | sed -e 's:.brite:.pre:'`

	touch ${output_dir}${out}

	#generating a preprocessing file based on a kmb algorithm
	./build/kmbopt $input_dir${i} $output_dir$out
	cat ${output_dir}${out}

	

done
