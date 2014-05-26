#!/bin/bash

#you must enter the instance's directory as the first parameter
path=$1
instances=`ls -v $1`
bin=$2
output=$3

for i in ${instances}
do
	#getting the name of the file
	file_=`basename $path/$i`
	file_=`basename $file_ .brite`

	#computing the limit
	result=`./${bin} $path/$i`

	#outputting the result
	echo $file_ $result >> $output/$file_.limit

done
