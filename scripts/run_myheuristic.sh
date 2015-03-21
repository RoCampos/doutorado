#!/bin/bash

#path of instances and preprocessed files
instance=$1
for i in `ls -vB ${instance}`
do
	#file=`echo $i | sed -e 's:.brite:.pre:'`
	#inst=$1n30/$i
	#pre=$1preprocessing/n30/${file}


	./build/acompp ${instance}/${i} --iter 500 --alpha 3.37 --beta 0.36 --evo  0.03 --pheur 0.52 --local_upd 0.29
done

