#/bin/bash

: '
	This script is used to perform the parameter tests of acompp.
	It will used to find the best values for five parameters used
	in the acompp algorithm.

	The parameters are:
	- iterations: the number of iterations of the algorithm.
	- m_alpha: parameter of the Ant Colony Optimization (ACO)
	- m_betha: parameter of ACO.
	- m_phe_rate: used to increase the value of a select edge. 
	- m_heuristic_prob: the value prob used to choose the component.


	the output file format used is:
		iterations_malpha_mbetha_mpherate_mheuristicprob.txt

	There ara some arguments of the scritpt:
	- bin: the binary that will run
	- input_dir: directory where the instance are stored
	- output_dir: diretoctory where the output files are stored
	- runs: number of executions of each parameter combinations


	run the script
	./scripts/aco_parameter_adjust.sh ./build/test_properties ../Result_Tests/aco-param/inst/ ../Result_Tests/aco-param/output/  10
		bin: ./build/test_properties
		input_dir: ../Result_Tests/aco-param/inst/
		output_dir: ../Result_Tests/aco-param/output/
		runs: 10	
		
'

bin=$1
input_dir=$2
output_dir=$3
runs=$4

iterations="100 500 1000 1500 2000 2500 3000"
malpha="0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9"
mpherate="0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9"
mheuristicprob="0.1 0.2 0.3 0.4 0.5 0.7 0.8 0.9"

#total runs = 7 * 9 * 9 * 9 = 7 * 9³ = (5103 combinations * 30 times)


#testing all the iteration values
for iter in ${iterations} 
do
	#testing all alpha values of the list malpha
	for alpha in ${malpha}
	do
		#testing all betha values of the list mbetha
		betha="0`echo "1 - $alpha" | bc`"
		
		for mp in ${mpherate}
		do

			#testing all the falues of mheuristicprob
			for h in ${mheuristicprob} 
			do
				
				for inst in `ls -vH $input_dir`
				do
					#number of execution
					for i in $(seq ${runs})
					do
				
						file_=`echo $inst | sed -e 's:.brite::'`
						out=$file_"_"acopar_$iter"_"$alpha"_"$betha"_"$mp"_"$h".txt"

						echo "Running: " $out;
					
						$bin $input_dir$inst $iter $alpha $betha $mp $h >> $output_dir$out
						sleep 1

					done
				done
			done

		done
		
	done

done
