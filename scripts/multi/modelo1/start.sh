#!/bin/bash

#brite instance
INSTANCE=$1

OPTION=$2

name=`basename $INSTANCE .brite`

if [ $OPTION == "all" ] || [ $OPTION == "gen" ];
then
	mkdir ${name}_lp
	#GENERATING LP FILES 
	./genlp.sh $INSTANCE ${name}_lp
fi

if [ $OPTION == "all" ] || [ $OPTION == "run" ]; then
	#statements
	mkdir ${name}_out
	#RUN GUROBI ALGORITHM
	./run_inst.sh ${name}_lp $INSTANCE ${name}_out
fi

if [ $OPTION == "all" ] || [ $OPTION == "plot" ]; then

	#runs a rscript to generate the plot of non-dominated solutions
	_time_=`cat ${name}_out/TIME`
	/usr/bin/Rscript plot_pareto.R ${name}_out/R ${name}_out/${name}.pdf ${_time_} ${name}
fi