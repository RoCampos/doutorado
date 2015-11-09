#!/bin/bash

# Este script é utilizado para rodar os lp´s no solver gurobi.

# O script executa os lp´s referentes a uma instância 'brite'.
# Em seguida alguns filtros são aplicados para obter a solução:
# 	- estrutura que compõe a solução: 'opt'
# 	- valor de tempo consumido pelo algoritmo: 'TIME'
# 	- valor objective de cadad lp: RESUMO
# 	- arquivo de entrada para script R

# O script recebe como entrada 3 argumentos:
# 	INPUT_DIR = diretório onde se encontram os lp´s
# 	INSTANCE = instância 'brite'
# 	OUTPUT_DIR = diretório onde os arquivos gerados pelo solver são
# 				 armazenados
#
# Execução do script
# ./run_inst.sh DIR_lp INSTANCE DIR_out
# DIR_lp	=	direorio onde se encontram os arquivos lp´s
# INSTANCE	=	instância brite referente aos arquivos brite
# DIR_out	= diretório onde são armazenados os resultados


INPUT_DIR=$1

INSTANCE=$2

OUTPUT_DIR=$3

total_time="0";

#sum=$((num1+num2+num3))
for lp in `ls -vH $INPUT_DIR`
do

	name=`basename $lp .lp`

	#running the solution
	echo "Running $name ..."
	gurobi_cl Threads=2 ResultFile=$OUTPUT_DIR/${name}.sol ${INPUT_DIR}/${lp} > $OUTPUT_DIR/${name}.info
	
	#filtering the solution
	grep 'y([0-9]*,[0-9]*,[0-9]*) [1]' $OUTPUT_DIR/${name}.sol > $OUTPUT_DIR/${name}.opt
	sed -i 's/y//g;s/[\(]//g;s/[\)]//g;0,/RE/s/,/ - /;s/,/:/g;s/ [1]$/;/' $OUTPUT_DIR/${name}.opt

	#Getting the line of result
	var=`grep 'Best objective' ${OUTPUT_DIR}/$name.info` 

	#getting time of all process gurobi
	time_=`grep -o '[0-9]*\.[0-9]* seconds' ${OUTPUT_DIR}/${name}.info | sed 's/ seconds//g;' | paste -s -d+ | bc`
	echo ${name} ${var} ${time_}>> $OUTPUT_DIR/RESUMO

	#getting time
	total_time=`echo "$total_time"+"$time_"`;

	#creating R file
	result=`echo $var | sed 's/[a-zA-Z]*//g;s/\+/e\+/g;s/\,//g;s/[0-9]*\.[0-9]*\%//g'`;
	brite=`basename $INSTANCE .brite`;	
	inst=`echo $lp | sed 's/[a-z]*[0-9]*_//g;s/.lp//'`;

	echo $inst $result >> $OUTPUT_DIR/R

	echo "... finished"
	
done  

echo $total_time | bc 

echo $total_time | bc >> $OUTPUT_DIR/TIME