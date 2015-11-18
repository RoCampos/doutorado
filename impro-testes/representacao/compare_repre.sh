#!/bin/bash


instance=$1

executavel=/home/romeritocampos/workspace/Doutorado/projeto/build/compare_steiner

for i in `ls -vH ${instance}`
do
	$executavel ${instance}/${i}
	echo $i
done
