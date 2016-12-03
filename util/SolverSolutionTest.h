#ifndef _SOLVER_SOLUTION_TEST_
#define _SOLVER_SOLUTION_TEST_

#include <iostream>
#include <string>
#include <cassert>
#include <memory>
#include <sstream>
#include <map>

#include "network.h"
#include "reader.h"
#include "group.h"

/**
* Classe template utilizada para implentar o policy design pattern
* para teste dos resultados obtidos por um solver.
*
* O argumento do template é uma implementação de fato de uma classe
* que interprete e teste o resultado do solver específico.
* 
* Para tanto, a classe Solver deve implementar o método @see do_test
* @see MMMSTPGurobiResult
*
* @author Romerito C. Andrade
*/
template <typename Solver>
class SolverSolution : private Solver
{
	
	using Solver::do_test;
	
public:
	
	/**
	* Método utilizar para executar o método do_test da classe Solver que é
	* argumento do classe template.
	* 
	* @param std::string instância cujo resultado será testado
	* @param std::string arquivo com resultado
	* @param int indica se será verboso(1) ou não(0).
	* @param bool 
	* @param std::string tipo da instância brite|yuh
	*/
	bool test (std::string instance, std::string result, int verb, std::string type) {		
		return do_test (instance, result,  verb, type);
	}
	
};

typedef std::vector<std::shared_ptr<rca::Group> >  list_groups;
/**
 * Classe implementa os testes necessários que devem ser aplicados ao resultado
 * do obtido pelo Otimizador gurobi. 
 *
 * Os testes realizados são:
 *  - valor de função objetivo
 *  - verificar se há uma árvore de steiner para cada grupo
 *  - valor de custo de uma solução  
 *  - conectividade de cada árvore
 *  - verifica se a nó folha não-terminal
 *  
 * Os arquivos de entrada devem possui o seguinte formato:
 * 
 * no1 - no2:arvore1;
 * no1 - no2:arvore2;
 * no1 - no2:arvoren;
 * 
 * no1 representa o primeiro nó de uma aresta, no2 representa o segundo nó de 
 * uma aresta. Arvore1 indica que a arestas (no1,no2) está nesta árvore. O valor
 * que indica a árovre varia de 1 até n(tamanho do grupo).
 * 
 * Todos as arestas da primeira árvore são listados, em seguida todas as aretas
 * da segunda árovore e assim por diante. A pontução e os espaços devem ser
 * respeitados. - sepera nós, : separa aresta do id da árver e ; encerra a linha.
 * 
 */
class MMMSTPGurobiResult {

public:

	/**
	* Método utilizado para realizar os testes.
	* Ele faz chamadas a métodos auxiliares para fazer testes 
	* nas árvores, para verificar valores de custo e capacidade residual.
	*
	* @param std::string instância cujo resultado será testado
	* @param std::string arquivo com resultado
	* @param int indica se será verboso(1) ou não(0).
	* @param bool 
	* @param std::string tipo da instância brite|yuh
	*/
	bool do_test (std::string instance, std::string result, int, std::string type);
	
private:
	//computa o valor objetivo com base nas árvores geradas
	int objective_test (rca::Network *,list_groups, std::string result);
	
	//faz os teste necessários para garantir que há uma árvore
	//de steiner para cada group
	int steiner_tree_test (rca::Network *,rca::Group *g, std::string result);
	
	int cost (rca::Network *, std::string result);
	
	//faz o teste de connectividade da árvore
	//considerando que todos os terminais devem estar no mesmo
	//disjoint set
	bool connectivity (rca::Group *g, DisjointSet2& dset, int numberOfnodes);
	
	
	/**
	 * Este teste verifica se há algum nó folha não terminal que 
	 * seja folha. Se for o teste considerado falho,  o resultado é inválido e
	 * o programa encerra.
	 */
	bool non_terminal_leaf_test (std::vector<int>&, rca::Group *g);
	
	int count_terminals (std::vector<int>&, rca::Group *g);
	
	int tree_cost (rca::Network *, rca::Group *g, 
					  DisjointSet2& dset, std::string);
	
private:
	int m_verbose;

};

#endif