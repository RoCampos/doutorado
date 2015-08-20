#!/bin/bash

# Este script é utilizado para gerar todas os possíveis LP´s
# para uma instâncias 'brite' passada como parâmetro.

# O script recebe como entrada um instânica 'brite' e um 
# diretório onde será armazena cada instâncias lp criada.

# O script possui duas funções 'get_max' e 'generate_lp':

# 	- A função 'get_max' é utilizada para o otimizar uma instância
# e obter o maior valor de capacidade residual possível.
# O valor obtido por 'get_max' é armazenado na variável 'MAX_RES'.

# 	- A função 'generate_lp' vai gerar todos os possíveis LP´s 
# para o problema. A contruçao do LP utiliza a ferramenta lpCostGenerator
# que gera um um problema de minimização da função objetivo
# sujeito a um valor mínimo de capacidade residual: 1 até 'MAX_RES'.

# Execução do script:
# 	- ./genlp.sh 'INSTANCE' 'out_lp'
# 	INSTANCE = instância utilizada para gerar os LP´s.
# 	out_lp = diretório de saída dos LP´s.

#instance (brite) to generate lps
INSTANCE=$1

#output directory
OUTPUT_DIR=$2;

#used to get maximal res of a instance
MAX_RES=0;

function generate_lp () {

	name=`basename $1 .brite`
	instance=${name}_$2.lp

	./lpCostGenerator $1 $2 > ${OUTPUT_DIR}/${instance}
}


# receives a brite instance as argument
function get_max () {

	name=`basename $1 .brite`

	./lpGenerator $1 1 > ${name}.lp
	gurobi_cl ${name}.lp >> ${name}.info

	#getting objective value
	var=`grep 'Best objective' $name.info`
	result=`echo $var | sed 's/[a-zA-Z]*//g;s/\+/e\+/g;s/\,//g;s/[0-9]*\.[0-9]*\%//g' | cut -d ' ' -f 3`;

	MAX_RES=`echo $result | sed 's/\./,/'` ;

}

#getting the better value of the $INSTANCE
get_max $INSTANCE
MAX_RES=`echo $(printf %0.f "$MAX_RES")`

#genereting LP
for max in `seq 1 $MAX_RES`
do
	echo "$INSTANCE with $max residual cap"
 	generate_lp $INSTANCE $max

done