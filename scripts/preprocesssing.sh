#!/bin/bash

#preprocessamento necessário para entrada no algoritmo de Chen.
#Vai gera um arquivo com o nome da instancia e extensão .pre

instances=`ls -v $1`
input_dir=$1
output_dir=$2
bin=$3

#sed expression to get the name of the file
#b30_19.brite_st_9.dat.log | sed -e 's:[.][a-z].*::'

for i in ${instances}
do

	file_=`echo $i | sed -e 's:[.][a-z].*::'`

	echo "Processing ${file_}"

	./${bin} ${input_dir}/${i} $output_dir/${file_}.pre
	echo "" >> $output_dir/${file_}.pre

done
