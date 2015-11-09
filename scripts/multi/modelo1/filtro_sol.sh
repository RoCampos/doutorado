#!/bin/bash

#diretório onde se encontrar arquivos *.sol
sol_instances=$1

#direorio onde serão colocados arquivos *.opt
output=$2

for inst in `ls ${sol_instances} --hide *.info`
do


	tmp=`basename ${inst} .sol` 
	echo ${tmp}
	./bin/filtro ${sol_instances}/$inst > ${output}/${tmp}.opt;

	#sort in reverse order(by opt)	
	#joga conteúdo em arquivo temporário (tmp)
#	rev < ${output}/${tmp}.opt | sort -n -k2 | rev > ${output}/${tmp}.tmp;
	
	#após sort joga o conteúdo novamente em opt.
#	cat ${output}/${tmp}.tmp > ${output}/${tmp}.opt;
	
	#removing duplicated lines and storing in *.tmp
#	uniq ${output}/${tmp}.opt > ${output}/${tmp}.tmp;

	#creating the file with optimal trees
#	cat ${output}/${tmp}.tmp > ${output}/${tmp}.opt;

	#remove arquivo temporário
#	rm ${output}/${tmp}.tmp;



done
