#/bin/bash

#exemplo de entrada
#/scripts/compute_optimal_cost.sh ../MPP_instances/n30/ ../MPP_instances/preprocessing/n30/ ../MPP_instances/optimal/


#directory where the intances are
instances=`ls -vH $1`

#directory where the preprocessed files are
preprocessed=`ls $2`

#directory where the result are stores
output=$3

for i in ${instances};
do

	pre=`echo $i | sed -e 's:.brite:.pre:'`
	out=`echo $i | sed -e 's:.brite:.opt:'`
	./build/optimalmpp $1$i $2$pre >> $output$out

done
