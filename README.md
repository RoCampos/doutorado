New Version - Improvements and Bug Fixed (05/25/2015)
========================================================
=ChenReplaceMethod
	- o método foi modificado para aumentar a velocidade.
	- É possível não obter o melhor custo, mas a velocidade
	aumentou consideravelmente.

=YouhChen's algorithm
	- Modificação do algoritmo para receber o visitor no fim
	de sua execução.

=GRASP
	- Arestas são removidas antes de construir uma árvore
	- remove-se apenas arestas mais utilizadas e que manténham
	o grafo conectado para a próxima árvore.


New Version - Improvements and Bug Fixed (04/26/2015)
========================================================
=ChenReplaceMethod by cost
	- Adicioonado ao visitor

New Version - Improvements and Bug Fixed (04/20/2015)
========================================================
=Youh-ChenAlgorithm
	- implementação do algotimo de youh-chen

New Version - Improvements and Bug Fixed (04/20/2015)
=========================================================
=sttree_to_path:
	- algoritmo que recebe uma árvore de Steiner(STTree)
	e retorna uma lista de caminhos para atualizar
	as estruturas do algoritmo genético.

=GeneticAlgorihtm
	- o genótipo é atualizado quanto se faz crossover
	e mutação.
	utiliza o sttre_to_path algorithm.

=ChenReplaceVisitor
	- Bug fixado.
	- o bug está ocorrendo na STTree(copy constructor)

= STTree
	- bug fixed.

=rcatime
	- interface redefinda.
	- incorporado no genetic e acommpp

New Version - Improvements and Bug Fixed (04/20/2015)
=========================================================
=LocalSearch
	- Busca local baseada no ChenReplaceMethod added
	to GeneticAlgorithm

New Version - Improvements and Bug Fixed (04/14/2015)
=========================================================
=ShortestPath
	- a efficient shortest path algorithm was implemented
	this algorithm improve the runtime of routines like GA

=SteinerTreeObserver
	- this idea was implemented on genetic_algorithm directory
	- It was removed to stalgorithm directory and now is distributed
	in sttalgo lib
	- Uses the container EdgeContainer from graph lib as default
	- support others containers for edge

=GeneticAlgorithm
	- the genetic algorithm now uses STTree to represent a steiner tree
	- The Steiner Tree observer was refactored and now is more efficient


New Version - Improvements and Bug Fixed (04/10/2015)
==========================================================
=EdgeContainer
        - É agora uma classe template, pode implementar um heap min ou max.

        - está no diretório network. Incorporado na libgraph

=Acompp
        -Utiliza a classe template EdgeContainer em substituição a classe EdgeContainer
        anterior

        -Árvores criadas aleatoriamente, não mais na sequência 0,1,2,...,n-1.

        - Utiliza nova class STTree para manter uma árvore de Steiner.

        - Aceita o ChenReplaceVisitor para substituir arestas

=ChenReplaceVisitor
        - Este visitor é utilizado para melhorar uma solução do MPP.

       -  Ele consiste em substituir arestas congestionadas por outras menos
        congestionadas.

=STTree
        - nova estrutura de dados que armazena uma árvore de Steiner. Bug free!

=Sttree_visitor
        - método prunnig implementado como um visitor. Útil para utilizar um
        EdgeContainer e atulizar o uso das arestas de forma eficiente.


New Version - Improvements and Bug Fixed (03/25/2014)
===========================================================
=Acompp
	- bug removido do método update_congestion () nos casos
	onde se controi uma solução sem usar arestas que foram utilizadas
	anteriomente.

	- Support a SolutionViewer: visualizar a construção das ároves durante
	a busca. Este recurso é ativido através de compilação condicional.

	

=SolutionViewer
	- Classe que representa um visualizador de solução utilizado no acompp.
	Este classe mantém um ponteiro para a rede e uma lista de arestas atualemente
	na solução. Estas arestas são atualizadas conforme inseridas ou removidas.
	Utiliza o formado do graphviz



New Version - Impromevents and Bug Fixed 03/22/2015 
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
