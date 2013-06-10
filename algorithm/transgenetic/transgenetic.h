#ifndef _TRANSGENETIC_H_
#define _TRANSGENETIC_H_

#include "heuristic.h"
#include "link.h"

class Transgenetic {

public:

	Transgenetic (int pop_size): m_popsize(pop_size){}

	//inicia as estruturas de dados
	void initialization ();

	//criar a população utilizando heuristica definida em heuristic.h
	void createPopulation ();

	void printPop ();

	//update cost if ST tree is better than the best, or better than
	//the solution in pos position.
	bool update (int pos, SteinerTree &);

	//plasmidio seleciona um caminho aleatoriamente entre dois terminais
	//aplica a uma solução
	void plasmidio (int);

	//este plasmidio aplica todos os caminhos entre dois terminais
	//para uma solução
	void plasmidio2 (int);

	//este plasmidio quarda os caminhos que melhoram a solução
	void plasmidio3 (int);

	//cria um pedaço de solução e adiciona na árvore 
	//referente ao parâmetro do método
	void plasmidioRecombinado (int);
	
	//local search based on vertex.
	void transposson (int);

	//loop principal do algoritmo
	void run (int);
	void run2 (int); //aplica todos os paths from i to j in only solution.
	void run3 (int, double); //aplica 2 with a probality	
	void run4 (int, double); //aplica plasmidio3 com prob
	void run5 (int); //aplica plasmidio recombinado

	//a probalidade é para o plasmidio3 ().
	void run6 (int, double); //aplica plasmidio3() e plasmidioRecombinado(); e plasmidio2 ()

	SteinerTree getIndividual (int pos){return m_population[pos];}

	void bestInfo ();
	
private:
	int m_popsize;
	std::vector<SteinerTree> m_population;

	std::set< rca::Path > m_best_paths;

	int m_best;
	double m_cost;

	int hasImproved;

};

#endif /*_TRANSGENETIC_H_*/
