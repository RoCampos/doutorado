03/22/2015 
============================================================
Esta versão contém as seguintes adições
=GeneticAlgorithm
	-Parâmetro local_upd é considerando na otimização(deve ser afinado)
	-Suporta compilação condicional para tratar rede com tk=1 ou tk=(variado)
	
=Acompp
	-Suporta compilação condicional para tratar rede com tk=1 ou tk=(variado)
	-Pode otimização para o modelo de congestão ou capacidade residual(objetivos)

=EdgeContainer
	- novo método para restaurar arestas removidas da rede. Considera o problema
	que envolve capacidade residual. Portanto, considera que uma quantidade da
	capacidade vai ser consumido por cada grupo (tk=1 ou tk=(variado)). O modelo
	que será usado (tk=1 ou tk(variado)) é transparente para este objeto.

=MultipleMulticastReader
	- Esta abstração é utilizada para ler informações do problema considerando que
	podem ser utilizados valores reais (conforme estão definidos na instância tk=vaiado)
	ou pode definir que tk=1 e todas as arestas terma capacidade igual ao tamanho 
	do grupo.


=DataGenerator
	- Suporta compilação condicional para gerar arquivos dat levando em consideração
	que se pode utilizar valores reais(conforme estão definidos na instância tk=vaiado)
	ou valores com tk=1 para cada grupo e todas as arestas terma capacidade igual ao tamanho 
        do grupo.

=SolverSolutionTest
	- Suporta compilação condicional para gerar arquivos dat levando em consideração
        que se pode utilizar valores reais(conforme estão definidos na instância tk=vaiado)
        ou valores com tk=1 para cada grupo e todas as arestas terma capacidade igual ao tamanho
        do grupo.

=MyRandom
	- New Method to reset the min, max interval for random number generation.
	This method holds the same semantic as before. But they increase the ACOMPP
	speed up.


Romerito PHD-1.
=======

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
