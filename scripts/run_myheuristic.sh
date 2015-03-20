#!/bin/bash

#path of instances and preprocessed files
instance=$1
for i in `ls -vB ${instance}`
do
	#file=`echo $i | sed -e 's:.brite:.pre:'`
	#inst=$1n30/$i
	#pre=$1preprocessing/n30/${file}


	./build/acompp ${instance}/${i} --iter 100 --alpha 1.24 --beta 8.52 --phe 0.89 --pheur 0.92 --local_upd 0.30
done

