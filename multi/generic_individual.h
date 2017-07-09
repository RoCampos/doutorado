
#ifndef _IND_OBJ_
#define _IND_OBJ_

#include "problem_info.h"
#include "individual.h"
#include "operator.h"

#include "steiner.h"
#include "steiner_tree_factory.h"
#include "steiner_tree_observer.h"
#include "sttree_local_search.h"
#include "localsearch.h"

/**
 * Esta estrutura é uma representação genérica
 * de um indivíduo. Nela é possível descrever a
 * representação de um indivíduo para um problema
 * específico ou importar uma implementação definida
 * em outra arquivo de cabeçalho.
 *
 */
typedef struct individual_t {
	
	~individual_t () {
		if (obj != NULL)
			delete obj;
	}

	/**
	 * Construtor padrão da Classe. Este construtor é utilizado
	 * para definir objetos nulos. O parâmetro é definido apenas
	 * para diferenciar do construtor que criar indivíduos para
	 * problema definidos pelo usuário.
	 */
	individual_t (bool);

	/**
	 * Construtor utilizado para criar indivíduos através de um mecanismo
	 * fornecido pelo usuário. O mecanismo é dependende do problema e deve
	 * ser implementado no escopo do construtor ou incluido através de um
	 * arquivo de cabeçalho.
	 */
	individual_t ();

	/**
	 * Esta função é utilizada para associar o indivíduo passado como
	 * parâmetro ao indivíduo que invoca a função. Este procedimento
	 * é util quando a necessidade de trocar indivíduos de posição no
	 * vetor que armazena a população.
	 *
	 * As informaçãos de individual_t são atribuídas ao objeto chamador.
	 *
	 * Retorna verdadeiro caso a atribuição seja realizada com exito.
	 *
	 * @param individual_
	 * @return bool
	 */
	bool assign (individual_t *); 

	void update_objectives () {
		this->obj[0] = this->ind.get_objective(0);
		this->obj[1] = this->ind.get_objective(1);
		this->obj[2] = this->ind.get_objective(2);

	}

	/**
	 * Usado para armazenar a posicao do indivíduo no vetor que
	 * representa a população
	 */
	int index;
	/**
	 * No Spea2 é usaudo como fitness. No Nsga2 é usado como rank.
	 */
	double fitness;

	/**
	 * Usado no Nsga2
	 */
	double crownding;


	double *obj;

	/**
	 * Objeto incluido na implementação do indivíduo genérico, este objeto
	 * indica que o indivíduo a ser utilizada é referente ao Multicast
	 * packing problem.
	 */
	Individual ind;
	
		
} GenericIndividual;

GenericIndividual::individual_t (bool val) {

#ifdef DEBUG_
	printf ("\nFunction %s\n", __PRETTY_FUNCTION__ );
#endif


	index = 0;
	fitness = 0.0;
	crownding = 0.0;
	obj = new double[Info::Problem::get_instance ()->OBJECTIVES];
	for (int i=0; i < Info::Problem::get_instance ()->OBJECTIVES; i++) {
		obj[i] = 0.0;
	}

	// this->mpp_ind = MulticastIndividual (2,Info::mproblem->getNumberGroups(),Info::mproblem);
	this->ind = Individual (Info::Problem::get_instance()->mgroups.size (), 3);

}

GenericIndividual::individual_t () 
{

#ifdef DEBUG_
	printf ("\nFunction %s\n", __PRETTY_FUNCTION__ );
#endif


	fitness = 0.0;
	index = 0;
	crownding = 0.0;

	obj = new double[Info::Problem::get_instance ()->OBJECTIVES];
	for (int i=0; i < Info::Problem::get_instance ()->OBJECTIVES; i++) {
		obj[i] = 0;
	}

	WidestPathTree stp1;
	ShortestPathTree stp2;
	BestFirstSearch stp3;

	int r = rand () % 30;


	/**
	* Inicie a configuração de seu objeto aqui
	*/
	this->ind = Individual (Info::Problem::get_instance()->mgroups.size (), 3);
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
			this->ind.add_tree (i, dtree);			
	}
	
	bool widest = true;
	if (widest) {
		std::vector<rca::Link> & ref = Info::Problem::get_instance () -> capacity;
		for (auto & l : ref) {
			Info::Problem::get_instance ()->network.setBand (l.getX(), l.getY(), l.getValue());
			Info::Problem::get_instance ()->network.setBand (l.getY(), l.getX(), l.getValue());
		}

	}

	evaluate (this->ind, 
		Info::Problem::get_instance ()->network, 
		Info::Problem::get_instance ()->mgroups);

	

	this->obj[0] = this->ind.objectives.at(0);
	this->obj[1] = this->ind.objectives.at(1);
	this->obj[2] = this->ind.objectives.at(2);
		
}

bool GenericIndividual::assign (individual_t * ind)
{	
	//verifica se o objeto já está no arquivo
	if (ind->obj[0] == obj[0] &&
		ind->obj[1] == obj[1] &&
		ind->fitness == fitness) return false;


	fitness = ind->fitness;
	index = ind->index;

	/**
	* ponha aqui alguma atribuição para o objeto que está
	* recebendo o ind.
	*/

	this->obj[0] = ind->ind.get_objective(0);
	this->obj[1] = ind->ind.get_objective(1);
	this->obj[2] = ind->ind.get_objective(2);

	this->ind = ind->ind;


	return true;
}

/**
 * Função para comparação de dois indivíduos pelo fitness.
 * Está função é util quando há necessidade de ordenar
 * os indivíduos de uma população através do fintess do mesmo.
 *
 * Nesta função os objetos são passados como ponteiros.
 *
 * @param GenericIndividual
 * @param GenericIndividual
 * @return bool
 */
bool compareByFitness(const GenericIndividual *ind1,
					 const GenericIndividual *ind2)
{
	return ind1->fitness < ind2->fitness;
}

/**
* Função para comparação de indivíduos por objetivo.
* Esta função é utilizada no Spea2 para cálculo da
* crowding distance.
* Pode se utiliza está função como objeto do algoritmo
* sort, por exemplo.
*/
struct compareByObjective {
	compareByObjective(int __obj) :objective(__obj) {	}

	bool operator() (GenericIndividual * ind1, GenericIndividual * ind2) {
		return ind1->obj[objective] < ind2->obj[objective];
	}
	int objective;
};


void crossover1 (Individual & p1, Individual &p2, Individual & ind)
{
	
	int NODES = Info::Problem::get_instance ()->network.getNumberNodes ();
	crossover_by_path (p1, p2, ind, NODES);
	evaluate (ind, 
		Info::Problem::get_instance ()->network, 
		Info::Problem::get_instance ()->mgroups);
}

void crossover2 (Individual & p1, Individual &p2, Individual & ind)
{
	int NODES = Info::Problem::get_instance ()->network.getNumberNodes ();
	crossover_by_hop (p1, p2, ind, NODES);
	evaluate (ind, 
		Info::Problem::get_instance ()->network, 
		Info::Problem::get_instance ()->mgroups);
}

void crossover3 (
	Individual & p1, 
	Individual & p2, 
	Individual & ind)
{
	crossover_by_tcost (p1, p2, ind);
	evaluate (ind, 
		Info::Problem::get_instance ()->network, 
		Info::Problem::get_instance ()->mgroups);
}

void crossover4(
	Individual &p1,
	Individual &p2,
	Individual &ind)
{
	crossover_by_edge (p1, p2, ind);
	evaluate (ind, 
		Info::Problem::get_instance ()->network,
		Info::Problem::get_instance ()->mgroups);
}

typedef rca::EdgeContainer<rca::Comparator, rca::HCell> Container;
typedef rca::sttalgo::SteinerTreeObserver<Container, steiner> Observer;
// typedef rca::sttalgo::cycle_local_search<Container, steiner> CycleLocalSearch;

void mutation (Individual & ind) 
{
	int SIZE = ind.cromossoma.size ();
	int NODES= Info::Problem::get_instance ()->network.getNumberNodes ();

	Container cg;
	cg.init_congestion_matrix (NODES);
	cg.init_handle_matrix (NODES);

	std::vector<steiner> trees;

	Observer observer;
	observer.set_container (cg);
	observer.set_network (
		Info::Problem::get_instance ()->network);

	int overcost = 0;

	for (int i = 0; i < SIZE; ++i)
	{
		rca::Group g = Info::Problem::get_instance ()->mgroups.at (i);		
		std::vector<int> const& members = g.getMembers ();
		int source = g.getSource ();
		int tk = g.getTrequest ();
		
		steiner st (NODES, source,  members);
		observer.set_steiner_tree (st, NODES);

		DTree & tree = ind.cromossoma.at (i);
		for (size_t k = 0; k < tree.paths.size (); ++k)
		{
			rca::Path & path = tree.paths.at (k);
			for (size_t p = 0; p < path.size ()-1; ++p)
			{
				int cost = 
					Info::Problem::get_instance ()->network.getCost (
						path.at (p), path.at (p+1));
				int band =
					Info::Problem::get_instance ()->network.getBand (
						path.at (p), path.at (p+1));
				rca::Link l (path.at (p), path.at (p+1), cost);

				observer.add_edge (l.getX(), l.getY(), cost, tk, band);
			}
		}	
		overcost += observer.get_steiner_tree ().get_cost ();
		trees.push_back (observer.get_steiner_tree ());
	}

	//performing cycle local search
	rca::sttalgo::CycleLocalSearch cls(
		Info::Problem::get_instance ()->network,
		Info::Problem::get_instance ()->mgroups,
		cg);	
	int z = cg.top ();
	
	int ocost = overcost;	
	do {
		ocost = overcost;
		cls.apply (trees, overcost, z);	 	
	} while (overcost < ocost);

	for (size_t i = 0; i < trees.size (); ++i)
	{
		rca::Group g = Info::Problem::get_instance ()->mgroups.at (i);		
		std::vector<int> const& members = g.getMembers ();
		int source = g.getSource ();

		DTree & curr_tree = ind.cromossoma.at (i);
		curr_tree.paths.clear ();

		for (size_t m = 0; m < members.size (); ++m)
		{
			std::vector<int> path = 
				trees.at (i).get_path (members.at (m), source);

			rca::Path p (path);
			curr_tree.paths.push_back (std::move(p));
		}
	}

	evaluate (ind,
		Info::Problem::get_instance ()->network,
		Info::Problem::get_instance ()->mgroups);

}

#endif
