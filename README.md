<h2>
New Version (1.0.0) - Implementações de modelos matemáticos (30/01/2016)	
</h2>

Modelos matemáticos agora pode ser solucionados por meio do gurobi_apiusando c++.
Assim fica, mas fácil resolver grandes modelos.

<dl>
	<dt>Modelos Matemáticos</dt>	
	<dd>
		Modelos implementados: BaseModel, ResidualModel, BudgetModel, LeeModel, LeeModifiedModel
	</dd>

	<dt>SteinerSolver</dt>
	<dd>
		O modelo de árvore de steiner foi implementado em c++ para rodar no solver
		gurobi.

		O modelo SteinerTreeModel permite obter o custo ótimo e o tamanho mínimo da árvore.
	</dd>
</dl>


<h2>
New Version (0.6.0) - Nova representação de árvore e novos algoritmos (09/12/2015)
=====================================================================
</h2>

<dl>
	<dt>steiner.h</dt>
	<dd>
		Nova represetação de árvore de Steiner baseada em grafo com lita de adjacência.
		steiner.h define a mesma interface que é definida em sttree.h, assim a compatilibidade
		é mantida.
	</dd>

	<dt>localsearch.h</dt>
	<dd>
		Algoritmos de busca local aplicados a solução com estrutura de árvore no modelo steiner.h

		Busca local baseada em Ciclo e na troca de arestas.
	</dd>

	<dt>mpp_visitor.h</dt>
	<dd>
		Classe MppVisitor e ChenReplaceVisitor são classes templates. Suportam diferentes tipo de 
		representação de árvore de Steiner: steiner.h ou sttree.h		
	</dd>

	<dt>sttree_visitor.h</dt>
	<dd>
		Vários funções foram modificados para funções templates.

		Otimização na função remove_top_edges ()

		Função prunning () é agora template.
	</dd>

	<dt>steiner_tree_factory.h</dt>
	<dd>
		Método shortest_path_tree foi modificado para criar árvore enraizadas em terminal ou na fonte, 
		isto é escolhido de forma aleatória.
	</dd>

	<dt>sttree_local_search.h</dt>
	<dd>
		CycleLocalSearch é agora uma classe template.
	</dd>

	<dt>Classes Templates</dt>
	<dd>
		Todos os algoritmos foram modificados para suportarem classes templates.
	</dd>

	

</dl>


<h2>
New Version (0.5.0) - Improvements, bug fixed and new procedures (11/10/2015) 
=====================================================================
</h2>

<dl>
	<dt>Documentação</dt>
	<dd>
		Praticamente todo o código fonte usado, direta ou indiretamente, foi documentado para facilitar distribuição, utilização
	</dd>

	<dt>
		GUROBI_SOLVER
	</dt>	
	<dd>
		Suporte a api do solver gurobi foi adicionada.
	</dd>

	<dt>
		Gerador de LPS
	</dt>
	<dd>
		O modelo base para formulação de outros modelos envolvendo múltiplas árvores de steiner recebeu método para adição de restrições.
	</dd>

	<dt>
		BUGS
	</dt>
	<dd>
		Modelo de LEE, gerador de arquivos dat's usados no glpsol por árvore.
	</dd>

</dl>

<h2>
New Version - Improvements, bug fixed and new procedures (07/27/2015)
=====================================================================
</h2>
=SteinerTreeFactory
	- implementa uma fábrica de objectos do tipo árvore de Steiner.
	Utiliza uma classe template para definir que tipo de objeto será gerado.
	- Há três algoritmos implementados: AGM, ShortestPath e WildestPath.

=Network
	- Há um novo método para recuperar o custo de um link. 
	Este método recebe como entrada um rca::Link.

=ChenReplaceVisitor
	- Este método considera que o link já está presente. Contém um parâmetro
	adicional para incrementar ou decrementar o valor do link.

=GeneticAlgorithm
	- Bug fixed on genetic algorithm.

=STTree_visitor
	- get_available_links: retorna os links que podem substituir outro em uma
	árvore passada como parâmetro

	- prints: métodos para imprimir uma solução utilizando diversos formatos.

	- replace_edge: método utilizado para susbstituir um link em uma árvore.


=EdgeContainer
	-Novo método para inserir um link no heap foi adicioando.

=Namespaces definition
	- foram definidos os seguintes namespaces: sttalgo, rca, etc.

=TabuSearch
	- uma versão do algoritmo tabu_search foi implementado.
	- Este algoritmo conta com diversas operações
	- possui dois tabus: um nas árvores que podem ser construídas
	e outro nos links que podem ser utilizados.


<h2>
New Version - Improvements and Bug Fixed (06/30/2015)
=====================================================
</h2>

=STTreeLocalSearch
	- cycle_local_search struct: usado para melhorar valor de
	custo de uma solução. Cria cíclos e depois remove as arestas que possuem 
	não-terminais.
	- improve_cost: reconstrução de solução baseado em árvore geradora mínima

=GeneticAlgorithm
	- O algoritmo foi modificado para melhor o tempo de processamento.
	O algoritmo utiliza o método all_shortest_path para construir uma árvore.

=STTreeVisitor
	- remove_top_edges: método que recebe um container e um objeto do tipo 
	Network e remove as arestas mais congestionadas.

=IntanceGenerator
	Utilizado para gerar informações de grupos para serem adicionads as
	topologias geradas pelo BRITE. Considera geração de informações sobre
	grupo utilizando valores fixos ou pencetuais.

=Heurísticas:
	- AGM: heurística baseada em árvore geradora com remoção de arestas mais congestionadas
	- SPH: heurística baseada em caminho mais curto com remoção de arestas mais congestionadas

=Modelo Matemático:
	- gerar modelo matemático com função objetivo otimizando o custo sujeito à restrição de capacidade
	residual.


<h2>
New Version - Improvements and Bug Fixed (05/25/2015)
========================================================
</h2>

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


<h2>
New Version - Improvements and Bug Fixed (04/26/2015)
========================================================
</h2>	

=ChenReplaceMethod by cost
	- Adicioonado ao visitor

<h2>
New Version - Improvements and Bug Fixed (04/20/2015)
========================================================
</h2>
=Youh-ChenAlgorithm
	- implementação do algotimo de youh-chen

<h2>
New Version - Improvements and Bug Fixed (04/20/2015)
=========================================================
</h2>
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

<h2>
New Version - Improvements and Bug Fixed (04/20/2015)
=========================================================
</h2>
=LocalSearch
	- Busca local baseada no ChenReplaceMethod added
	to GeneticAlgorithm

<h2>
New Version - Improvements and Bug Fixed (04/14/2015)
=========================================================
</h2>
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

<h2>
New Version - Improvements and Bug Fixed (04/10/2015)
==========================================================
</h2>
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


<h2>
New Version - Improvements and Bug Fixed (03/25/2014)
===========================================================
</h2>
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



<h2>
New Version - Impromevents and Bug Fixed 03/22/2015 
============================================================
</h2>
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


<h2>
Romerito PHD-1.
============================================================
</h2>


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
