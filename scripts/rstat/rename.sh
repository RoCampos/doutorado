#!/bin/bash

if [ $1 == "--help" ]; then

	echo "script que renomeia os arquivos substituindo b30_1.result por"
	echo "b30_01.result. A extensão pode ser qualquer uma."
	echo -e "Argumentos: \n\t1 Diretório onde estão as instâncias\n\t2 Extensão(.result, .brite)"

	echo -e "Example \n rename.sh . result \n where . is the directory where the instances are"

	exit
else 

instances=$1
extension=$2

x=1;
for i in `ls -vH ${instances}/*.${extension}`
do

	file=`echo ${i} | sed 's/\(.*\).[[:digit:]].result/\1/'`
	file=`echo ${file} | sed 's:_::'`


	if [ ${x} -lt 10 ];
	then 
		name=${file}_0${x}.result
	else 
		name=${file}_${x}.result
	fi

	mv ${i} ${name}
	let "x=x+1"
done


fi
