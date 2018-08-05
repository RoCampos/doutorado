#ifndef MOEAD_H
#define MOEAD_H

#include <vector>
#include <cstdlib>
#include <iostream>
#include <limits>

#include "multiobjective.h"
#include "problem_info.h"
#include "individual.h"
#include "operator.h"
#include "generic_individual.h"

namespace evolutionary {

struct individual_t
{
	int index;
	std::vector<double> objectives;
	Individual individual;

	individual_t () {
		int objs = Info::Problem::get_instance ()->OBJECTIVES;
		objectives.resize (objs);
		individual = Individual (
			Info::Problem::get_instance()->mgroups.size (), objs);
	}

	void str () {
		for (auto m : objectives) std::cout << m << " ";
		std::cout << std::endl;
	}

	void initialize () {

		objectives.resize (Info::Problem::get_instance ()->OBJECTIVES);

		WidestPathTree stp1;
		ShortestPathTree stp2;
		BestFirstSearch stp3;
		int r = rand () % 30;

		this->individual = Individual (Info::Problem::get_instance()->mgroups.size (), 3);
		for (size_t i=0; i < Info::Problem::get_instance()->mgroups.size (); i++) {
				int tk = Info::Problem::get_instance()->mgroups[i].getTrequest ();
				int src = Info::Problem::get_instance()->mgroups[i].getSource ();
				std::vector<int> members = Info::Problem::get_instance()->mgroups.at(i).getMembers ();
				DTree dtree;		
				members.push_back (src);
				int pos = rand () % members.size ();
				src = members.at (pos);
				members.erase (members.begin ()+pos);
				
				if (r < 10)
					stp1.execute (src, members, Info::Problem::get_instance ()->network, dtree.paths, tk);	
				else if (r >= 10 && r < 20)
					stp2.execute (src, members, Info::Problem::get_instance ()->network, dtree.paths);	
				else 
					stp3.execute (src, members, Info::Problem::get_instance ()->network, dtree.paths);	

				reorganize (src, pos, i, dtree, Info::Problem::get_instance()->mgroups, Info::Problem::get_instance ()->network);
				this->individual.add_tree (i, dtree);			
		}
		
		bool widest = true;
		if (widest) {
			std::vector<rca::Link> & ref = Info::Problem::get_instance () -> capacity;
			for (auto & l : ref) {
				Info::Problem::get_instance ()->network.setBand (l.getX(), l.getY(), l.getValue());
				Info::Problem::get_instance ()->network.setBand (l.getY(), l.getX(), l.getValue());
			}

		}

		evaluate (this->individual, 
			Info::Problem::get_instance ()->network, 
			Info::Problem::get_instance ()->mgroups);

		this->objectives[0] = this->individual.objectives.at(0);
		this->objectives[1] = this->individual.objectives.at(1);
		this->objectives[2] = this->individual.objectives.at(2);

	}

	void update () {
		int OBJ = Info::Problem::get_instance ()->OBJECTIVES;
		this->objectives.resize (OBJ);
		for (int i =0; i < OBJ; i++)
		{
			this->objectives[i] = this->individual.objectives.at (i);
		}
	}

};


/**
* Define um subproblema a ser otimizado.
* Melhor indivíduo do subproblema é uma solução
* presente no pareto.
*/
struct subproblem_t {

	individual_t repr;

	//vetor de pesos do subproblem
	std::vector<double> weight;

	//vizinhos do subproblem B(i) no texto da proposta
	std::vector<int> neighbour;

	//aptidão do indivíduo
	double fitness;

	void print () {

		std::cout << "objectives: ";
		for (auto & m : repr.objectives){
			std::cout << m << " ";
		}
		std::cout << std::endl;
		std::cout << "weighted vector:";
		for (auto & m : weight) {
			std::cout << m << " ";	
		}
		std::cout << std::endl;

	}

};

/**
* Esta estrutura é utilizada para organizar as informações
* de vizinho para cada subproblema.
*
*
*/
struct neighbour_t {

	int index;
	double distance;

	neighbour_t () {
		index = 0;
		distance = 0.0;
	}

	bool operator< (neighbour_t const & next) const {
		return distance < next.distance;
	}

	bool operator< (neighbour_t const & next) {
		return distance < next.distance;
	}

};

class MOEAD {

public:
	MOEAD (int _H = 12, int _T=5, int limit = 2, int iter=50) 
		: H(_H), T(_T), L(limit), ITER (iter) {
		s_idealpoint.resize (Info::Problem::get_instance ()->OBJECTIVES);
		s_idealpoint.at (0) = std::numeric_limits<double>::min();
		s_idealpoint.at (1) = std::numeric_limits<double>::max();
		s_idealpoint.at (2) = std::numeric_limits<double>::max();

		nadir.push_back (0);
		nadir.push_back (std::numeric_limits<double>::min());
		nadir.push_back (std::numeric_limits<double>::min());
	}
	~MOEAD () {}

	void initUniformWeight ();
	void initNeighbourhood ();
	void initPopulation ();
	void updateReferencePoint (int);
	void selectParents (int, std::vector<int>& indexes);
	void updateProblem (int index, subproblem_t & sub);
	void crossover ();

	void run ();
	
	void printPop () {
		for(auto p : m_population) {
			p.repr.str ();
		}

		std::cout <<"Ideal Point (";
		for (auto i : s_idealpoint) std::cout << i << " ";
		std::cout << ")\n";

		std::cout <<"Nadir Point (";
		for (auto i : nadir) std::cout << i << " ";
		std::cout << ")\n";
	}

	void printArc () {
		for(auto p : m_nondominated) {
			p.repr.str ();
		}		
	}

private:

	//TODO
	void fitnessAssigment (int index);

	void updateParetoFrontier ();

	int H; //parameter that controls the number of weighted vectors
	int T; //parameter that controls number of neighbours of weighted vector
	int L; //atualization problem limit
	int ITER; //max number of generation
	std::vector<subproblem_t> m_population;
	std::vector<subproblem_t> m_nondominated;

	std::vector<double> s_idealpoint, nadir;

};

void MOEAD::initUniformWeight () {


	int index = 0;
	for (int i = 0; i < H; ++i)
	{
		for (int j = 0; j < H; ++j)
		{
			if (i+j <= H) {

				subproblem_t moeadind;

				double w1, w2, w3;
				w1 = i/(float)H;
				w2 = j/(float)H;
				w3 = (H-i-j)/(float)H;

				moeadind.weight.push_back (w1);
				moeadind.weight.push_back (w2);
				moeadind.weight.push_back (w3);

				moeadind.repr.index = index;
				m_population.push_back (std::move(moeadind));

				index++;

			}
		}
	}
}

void MOEAD::initNeighbourhood () 
{

	std::vector<neighbour_t> neighbourhood (this->m_population.size ());

	for (size_t i = 0; i < m_population.size(); ++i)
	{
		//calculating distance in objective-space
		for (size_t j = 0; j < m_population.size(); ++j)
		{
			neighbourhood[j].index = j;
			neighbourhood[j].distance = MultiObjective::distanceCalc(
				m_population.at(i).weight,
				m_population.at(j).weight, 
				Info::Problem::get_instance()->OBJECTIVES);
		}

		//sorting neighbours
		std::sort (neighbourhood.begin(), neighbourhood.end());
		for (int t = 0; t < this->T; t++) {
			m_population[i].neighbour.push_back (neighbourhood[t].index);
		}

	}


}

void MOEAD::initPopulation () {

	for (size_t i = 0; i < m_population.size (); ++i)
	{
		m_population[i].repr.initialize ();
		this->updateReferencePoint (i);
	}

}

void MOEAD::updateReferencePoint (int index) 
{
	
	subproblem_t & ind = m_population.at (index);
	std::vector<double> & ref = ind.repr.objectives;

	if (ref.at (0) > s_idealpoint[0]) {
		s_idealpoint[0] = ref.at (0);
	}

	if (ref.at (1) < s_idealpoint[1]) {
		s_idealpoint[1] = ref.at (1);
	}

	if (ref.at (2) < s_idealpoint[2]) {
		s_idealpoint[2] = ref.at (2);
	}

	if (ref.at(1) > nadir.at (1)) {
		nadir.at (1) = ref.at(1);
	}

	if (ref.at(2) > nadir.at (2)) {
		nadir.at (2) = ref.at(2);
	}


}


void MOEAD::crossover ()
{

	std::vector<int> randomInd(m_population.size());

	std::iota(randomInd.begin(), randomInd.end(), 0);
	// std::mt19937{std::random_device{}()}
	std::random_shuffle (randomInd.begin(),randomInd.end());

	std::vector<int> selected;

	for (size_t i = 0; i < m_population.size (); ++i)
	{
		
		int n = randomInd[i];

		//seleção pode ser na vizinhança do ponto ou na população inteira
		//apenas na vizinhança
		selectParents (n, selected);

		subproblem_t & parent1 = m_population.at (selected.at (0));
		subproblem_t & parent2 = m_population.at (selected.at (1));
		subproblem_t newind;

		//its working must provide others way to perform crossover

		int prob_cross = rand () % 10 + 1;
		double p = ((double)prob_cross/10);
		if ( p <= 0.25 ) {			
			crossover2 (parent1.repr.individual,
			 parent2.repr.individual, 
			 newind.repr.individual);
		} else if ( p >= 0.25 && p < 0.5) {
		 	crossover4 (parent1.repr.individual, 
		 		parent2.repr.individual, 
		 		newind.repr.individual);
		} else if ( p >= 0.5 && p < 0.75) {
			crossover1 (parent1.repr.individual, 
				parent2.repr.individual, 
				newind.repr.individual);
		} else {
			crossover3 (parent1.repr.individual, 
				parent2.repr.individual, 
				newind.repr.individual);	
		}

		newind.repr.update ();		
		updateProblem (n, newind);

		prob_cross = rand () % 10 + 1;
		p = ((double)prob_cross/10);
		if (p < 0.2)
			mutation (newind.repr.individual);

		// newind.repr.individual.str ();

	}

}

void MOEAD::selectParents (int ind, std::vector<int>& indexes)
{
	
	std::vector<int> & neighbours = m_population.at (ind).neighbour;

	int ind1 = rand () % neighbours.size ();
	int ind2 = rand () % neighbours.size ();
	while (ind1 == ind2) ind2 = rand () % neighbours.size ();

	indexes.push_back (ind1);
	indexes.push_back (ind2);

}

void MOEAD::updateProblem (int index, subproblem_t & sub)
{

	int SIZE = m_population.at (index).neighbour.size ();
	std::vector<int> randomInd(SIZE);
	std::iota(randomInd.begin(), randomInd.end(), 0);
	std::random_shuffle (randomInd.begin(),randomInd.end());	

	int * objconf = Info::Problem::get_instance () ->objconf;

	int replaces = 0;
	for (int i = 0; i < SIZE; ++i)
	{
		
		//random neighbour of index
		int n = m_population.at (index).neighbour.at ( randomInd.at (i) );

		//fitness based on tchebycheff
		double f1, f2;
		std::vector<double> obj1 = m_population.at(n).repr.objectives;
		std::vector<double> & lamb1 = m_population.at(n).weight;
		MultiObjective::normalization (obj1, nadir ,s_idealpoint, objconf);		
		f1 = MultiObjective::thebycheffWeighet (
			obj1, lamb1);

		std::vector<double> obj2 = sub.repr.objectives;
		MultiObjective::normalization (obj2, nadir ,s_idealpoint, objconf);
		f2 = MultiObjective::thebycheffWeighet (
			obj2, lamb1);

#ifdef DEBUG_
		m_population.at(n).repr.str ();
		sub.repr.str ();
		cout << f1 << " " << f2 << endl;
		for(auto&& m : lamb1) std::cout << m << " ";
		std::cout << std::endl;
		for(auto&& m : s_idealpoint) std::cout << m << " ";
		std::cout << std::endl;

		std::cout << std::endl;
#endif

		if (f2 < f1) {
			m_population[n].repr = sub.repr;
			replaces++;
		}

		if(replaces>=this->L) {
			break;
		}

	}

}

void MOEAD::updateParetoFrontier ()
{

#ifdef DEBUG_
	std::cout << __FUNCTION__ << std::endl;
#endif

	double * obj1 = new double[3];
	double * obj2 = new double[3];
	int * objconf = Info::Problem::get_instance ()->objconf;
	int OBJ = Info::Problem::get_instance ()->OBJECTIVES;

	for (size_t i = 0; i < this->m_population.size (); ++i) {

		obj1[0] = m_population[i].repr.objectives.at (0);
		obj1[1] = m_population[i].repr.objectives.at (1);
		obj1[2] = m_population[i].repr.objectives.at (2);

		bool dom = true;
		std::vector<int> remove;		
		for (size_t j = 0; j < m_nondominated.size(); ++j) {

			obj2[0] = m_nondominated[j].repr.objectives.at (0);
			obj2[1] = m_nondominated[j].repr.objectives.at (1);
			obj2[2] = m_nondominated[j].repr.objectives.at (2);

			//avoid repeated ind
			if (MultiObjective::equals (obj1, obj2, OBJ)) {
				dom = false;
				break;
			}

			//se domina ok.
			if (MultiObjective::dominate(obj1, obj2, objconf))
			{
				dom = true;
				if (!MultiObjective::dominate(obj2, obj1, objconf)) {
					remove.push_back (j);
				}
				//senão, então testa se é dominado
			} else if (MultiObjective::dominate(obj2, obj1, objconf)) 
			{
				//sendo então pára
				dom = false;
				break;
			}

		}
		//print nonminated
		if (dom) {
			// cout << m_nondominated.size () << endl;			
			auto iter = remove.rbegin ();
			for (; iter != remove.rend(); iter++) {
				m_nondominated.erase (m_nondominated.begin()+(*iter));
			}
			m_nondominated.push_back (m_population.at(i));
		}
	}//fim loop over i
	// std::cout << std::endl;

	delete obj1;
	delete obj2;
	objconf = NULL;

}

void MOEAD::run () {

	this->initUniformWeight ();
	this->initNeighbourhood ();
	this->initPopulation ();
	this->updateParetoFrontier ();
	for (int i = 1; i < this->ITER; ++i)
	{		
		
		this->crossover ();
		this->updateParetoFrontier ();

	}



}

} // evolutionary



#endif // MOEAD_H
