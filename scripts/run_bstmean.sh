#!/bin/bash

#directory of instances
instance=$1

#list of means used
list="1.05 1.1 1.5 1.2 1.25 1.3"
for mean in ${list}
do
	output=$instance
	for i in `ls -vB ${instance}/n30/`
	do
		file=`echo $i | sed -e 's:.brite:.pre:'`
		inst=$1n30/$i
		pre=$1preprocessing/n30/${file}

		./build/bsmean ${inst} ${mean} >> $output'bstmean_'$mean
	done


done
