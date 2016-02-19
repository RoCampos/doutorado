#!/bin/bash

# Script para rodar os modelos LEE e LEE_M
# LM -- recebe os opt de custo
# LMM -- recebe os opt de size

# CUSTO
# cut -f1 -d' ' ${opt_file}/${file}.opt > file.opt
# ./mathmodel --model LM --instance ${instance_dir}/${inst} --opt file.opt --alpha $ALPHA --log ${file}.log 2> ${file}.result


# SIZE
# cut -f2 -d' ' ${opt_file}/${file}.opt > file.opt
# ./mathmodel --model LMM --instance ${instance_dir}/${inst} --opt file.opt --alpha $ALPHA --log ${file}.log 2> ${file}.result


#diretório das instâncias a serem otimizadas
instance_dir=$1

#arquivos ótimos por tamanho/custo
opt_file=$2

#resultado será direcionado para este diretórios
output=$3

ALPHA=$4

for inst in `ls ${instance_dir} -vH`
do

	#getting the filename of instance
	file=$(basename ${inst} .brite)

	echo "Running ${file}"
	echo "Running ${file}" >> log


	#getting the optimal cost/size
	cut -f1 -d' ' ${opt_file}/${file}.opt > file.opt

	#getting 
	./mathmodel --model LM --instance ${instance_dir}/${inst} --opt file.opt --alpha $ALPHA --log ${file}.log 2> ${file}.result

	mv file.log ${file}.log
	mv modelo.lp ${file}.lp	

	exit

done

rm file.log file.opt modelo.lp