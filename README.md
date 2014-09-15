Romerito PHD-1.
======

This version is the initial point of my investigation
about the problem: Multicast packing problem(MPP).

In this version I've implemented a aco algorithm to solve
the MPP.

The central idea of the algorithm is to put n ants in the graph
one for each terminal node. The ants will move until all were 
togeher in the same "route". By this I mean that the ants will
move from a vertex to another vertex. When a ant find another 
ant the will work together from this point. 

The stop condition occurs when all the ants are together.

For each group multicast the idea is applied. In the final
of construction of multicast trees, the pheroment matrix is updated.

Algorithms:
	Chen 200: this algorithm will be compared against my ideas.
	myalgoritm: my aco alagorithm.
	bsmean: heuristic used in the aco algorithm.

Beside the algorithm, some improvement has been done in the source
code used as infraestructure of the reaserch. 


