#/bin/bash

#path of instances and preprocessed files
instance=$1
for i in `ls -vB ${instance}/n240/`
do
	file=`echo $i | sed -e 's:.brite:.pre:'`
	inst=$1n240/$i

	#used for get preprocesed files obtained by solver
	pre=$1preprocessing/n240/${file}

	#used for get the processed files using kmb algorithm
	pre=$1kmbpreprocessing/n240/${file}

	./build/chentest ${inst} ${pre}
done

