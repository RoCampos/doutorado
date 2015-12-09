#!/bin/bash

#algorithm 1
algo1=$1

#algorithm 2
algo2=$2

#tamanho instância(b30, b60, b120, 240)
instsize=$3

for id in `ls -vH $algo1`
do

	#file=${instsize}_${id}
	file=${id}
	#echo $file
	#echo "sample 1 :"
	#cat ${algo1}/$file
	#echo "sample 2"
	#cat ${algo2}/$file

	/usr/bin/Rscript dostat.R ${algo1}/$file ${algo2}/$file

#	exit


done
