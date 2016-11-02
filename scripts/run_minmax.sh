#!/bin/bash

path=~/Documentos/Doutorado/mmmrp/exp5/exp5_1/exp5_1_2/data/

for inst in `ls ${path} -vH`; do

	./build/minmax --int ${path}/${inst} --rem 0.78 --reverse yes --sort request --local local --single no --result full

done