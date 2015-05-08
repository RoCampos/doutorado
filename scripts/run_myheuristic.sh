#!/bin/bash

#path of instances and preprocessed files
instance=$1

#directory where the result files will be stored
output_dir=$2

budget_file=$3
list=( `cat ${budget_file}` )

idx=0

for inst in `ls -vB ${instance}`
do
	#file=`echo $i | sed -e 's:.brite:.pre:'`
	#inst=$1n30/$i
	#pre=$1preprocessing/n30/${file}

	#getting the basename of a file. Name with no extension
	file=$(basename ${inst} .brite)

	#echo ${list[ ${idx} ]}

	echo -e "CAP_R\tCUSTO\tITER\tTIME\tSEED" > ${output_dir}/${file}.result
	for i in  $(seq 1 50)
	do
		./build/acompp ${instance}/${inst} --iter 400 --alpha 2.34 --beta 1.31 --evo  0.34 --pheur 0.28 --local_upd 0.07 >> ${output_dir}/${file}.result ${list[ ${idx} ]}
	done

	let "idx = idx + 1"
done
