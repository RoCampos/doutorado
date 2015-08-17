#!/bin/bash

#cria modelo de dados modelData do projeto de doutorado
MODEL_DATA=$1

#modelo de steiner presente em mathprogramming: stp_multi.mod
STEINER_MODEL=$2

#diretório onde estão as instâncias da árvore de steiner
INSTANCE_DIR=$3


#---------------------------------- FUNCTION
# Está função é utilizada pra criar um arquivo 'stp' para cada
# árvore multicast prensente em uma instância 'brite'.
# Dada uma instância brite 'b30_1.brite' o a função
# vair criar um diretório 'b30_1' e dentro deste
# diretório serão criados arquivos 'b30_1_0.stp', 'b30_1_1.stp' ...
# um para cada grupo multicast na instância passada como parâmetro
function multicast_steiner() {

	instance=$1;
	
	name=`basename ${instance} .brite`;

	mkdir ${name}
	
	#calling the binary who create .stp files
	# recebe uma instância
	# o valor indica que usará a opção 3 de modelData
	./${MODEL_DATA} $instance 3 .

	mv *.stp ${name}/

	cd ${name}/

	#renaming files from tree to B30_1_0.stp for example
	for file in `ls *.stp` 
	do
		#replacing tree_<tid>.stp to b<size>_<id>_<tid>.stp
		temp=`echo $file | sed s/tree/$name/`;
		mv $file $temp;
	done

	cd ../
	
}

# Resolve o problema da árovre de steiner para cada grupo multicast 
# associada a uma instância 'brite'.
#
# recebe como parâmetro o nome da instância brite sem a extensão .brite
# 'b30_1.brite' é passado como parâmetro da seguinte forma 'b30_1'
#
# Dentro do diretório 'brite' haverá um um arquivo 'b30_1.sopt' com
# o valor objetivo de cada uma das árvores na ordem crescente k=0, 1, 2...
function run_steiner_model() {

	#directory of instance of stp	
	instance_dir=$1
	name=`basename ${instance_dir} .stp`
	for i in `ls -vH ${instance_dir}/*.stp`
	do

		#running glpsol with STEINER_MODEL		
		glpsol --math ${STEINER_MODEL} --data ${i} | grep 'stpd.val' | cut -d ' ' -f 3 >> ${instance_dir}/${name}.sopt

	done

}

#---------------------------------- END OF FUNCTIONS



#running the script

for inst in `ls -vH ${INSTANCE_DIR}`
do

	#creating stp files	
	multicast_steiner $INSTANCE_DIR/$inst

done


for inst in `ls -vH ${INSTANCE_DIR}`
do

	#running glpsol
	name=`basename $inst .brite`
	run_steiner_model $name
done




