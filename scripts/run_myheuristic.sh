#!/bin/bash

#path of instances and preprocessed files
instance=$1
for i in `ls -vB ${instance}/n120/`
do
	file=`echo $i | sed -e 's:.brite:.pre:'`
	inst=$1n120/$i
	pre=$1preprocessing/n120/${file}


	./build/test_properties ${inst}
done

