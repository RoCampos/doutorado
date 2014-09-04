#!/bin/bash

#path of instances and preprocessed files
instance=$1
for i in `ls -vB ${instance}/n30/`
do
	file=`echo $i | sed -e 's:.brite:.pre:'`
	inst=$1n30/$i
	pre=$1preprocessing/n30/${file}


	./build/test_properties ${inst} 100
done

