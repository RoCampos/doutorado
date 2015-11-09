#ifndef _SOLVER_DATA_GENERATOR_H_
#define _SOLVER_DATA_GENERATOR_H_

#include <iostream>
#include <ostream>
#include <sstream>
#include <memory>

#include <fstream>

#include "network.h"
#include "group.h"


/**
 * Gerador de dados que recebe como parâmetro de 
 * template o modelo a ser utilizado.
 * 
 * É possível trocar de modelo através da parametrização
 * da classe DataGenerator.
 * 
 * Exemplos de modelos são:
 * 	- @MultiCommidityFormulation usado para gerar dados no formato
 * dat para o problema da árvore de Steiner. Recebe um @Group e um objeto
 * representando a rede(@Network). Os arquivos dat pode ser utilizados em conjunto
 * com o modelo stp_multi.mod.
 * 
 *  - @MultipleMulticastCommodityFormulation usado para gerar dados no formado
 * dat para o problema Multiple Multicas Packing que maximiza capacidade residual.
 * Esta versão não possui orçamento. Os arquivos dat deve ser utilizados em
 * conjunto com o modelo mmmstp.mod.
 * 
 *  - @MultipleMulticastCommodityLP é usado para gerar arquivos no formato
 * LP. Esta versão é a implementação do modelo mmmstp.mod. Os arquivos LP são 
 * gerados diretamente por código c++, ou seja, não é necessário utilizar arquivos
 * dat e o solver.
 * 
 *  - @MMSTPBudgetLP é usado para gerar arquivos no formato LP. Esta é a implementação
 * do modelo mmmstp2.mod. Os arquivos LP são gerados diretamente por código c++, 
 * ou seja, não é necessário utilizar arquivos dat e o solver. Adiciona restrição
 * de orçamento.
 * 
 * 
 * Todos as classes consideram a capacidade real(definida nas instâncias). Além diss
 * a TRequest de @Group é também utilizado como definida nas instâncias.
 * 
 * @author Romerito Campos de Andrade
 * @date 03/19/2015
 */
template <typename Model>
class DataGenerator : private Model
{
	using Model::generate;
	
public:
	
	/**
	* Método utilizado para gerar os dados no formato .dat utilizado pelo glpsol
	* @param rca::Network rede onde a solução é gerada
	* @param rca::Group grupo multicast
	*/
	void run (rca::Network * network, rca::Group *g) {		
		generate (network, g);
		
	}
	
	void run (rca::Network * network, std::vector<std::shared_ptr<rca::Group>>& g) {
		generate (network, g);
	}
	
	
};

/** 
 * @MultiCommidityFormulation usado para gerar dados no formato
 * dat para o problema da árvore de Steiner. Recebe um @Group e um objeto
 * representando a rede(@Network). Os arquivos dat pode ser utilizados em conjunto
 * com o modelo stp_multi.mod.
 *
 *
 *
 * As árvores são armazenadas em arquivos e escritas no diretório local
 * 
 */
class MultiCommidityFormulation 
{
	
public:
	
	/**
	* Gera os dados no formato dat. 
	*
	* Exemplo:
	*
	*	V := 1 2 3 4 5;
	* 	E := V cross V;
	*	...
	*
	* @param rca::Network rede onde a solução é gerada
	* @param rca::Group grupo multicast
	*/	
	void generate (rca::Network * network, rca::Group *g) ;
	
	/**
	 * Esta variável controla as árvores que estão sendo criadas.
	 * Inicia a contagem na árvore 0.
	 */
	static int m_tree;
	
};

/** 
 * Classs MultipleMulticastCommodityFormulation usado para gerar dados no formado
 * dat para o problema Multiple Multicas Packing que maximiza capacidade residual.
 * Esta versão não possui orçamento. Os arquivos dat deve ser utilizados em
 * conjunto com o modelo mmmstp.mod.
 *
 *
 *
 */
class MultipleMulticastCommodityFormulation
{

public:
	void generate (rca::Network * network, 
					std::vector<std::shared_ptr<rca::Group>>&);
	
};


/**
 * @MultipleMulticastCommodityLP é usado para gerar arquivos no formato
 * LP. Esta versão é a implementação do modelo mmmstp.mod. Os arquivos LP são 
 * gerados diretamente por código c++, ou seja, não é necessário utilizar arquivos
 * dat e o solver.
 */
class MultipleMulticastCommodityLP 
{
	
public:
	virtual void generate (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
protected:
	
	/**
	* Restrição de fluxo 1
	*
	* \f$ \sum_{i \in V}{x_{ir_k}^{kd}} - \sum_{i \in V}{x_{r_{k}i}^{kd}} = -1 \quad \forall k \in K, \forall d \in D^k \f$
	*
	* @param rca::Network ponteiro para rede utilizada
	* @param std::vector<std::shared_ptr<rca::Group>> grupos multicast utilizados
 	*/
	void constraint1 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);

	/**
	*
	* Restrição de fluxo 2
	*
	* \f$ \sum_{(i,j) \in E}{x_{ij}^{kd}} - \sum_{(i,j) \in E}{x_{ji}^{kd}} = 0 \quad \forall k \in K, \forall d \in D^k \label{lab2} \f$
	* 
	* @param rca::Network ponteiro para rede utilizada
	* @param std::vector<std::shared_ptr<rca::Group>> grupos multicast utilizados
	*/
	void constraint2 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	/**
	* Restrição de fluxo 3
	*
	* \f$ \sum_{(i,j) \in E \atop j \neq r_k,d}{x_{ij}^{kd}} - \sum_{(i,j) \in E \atop j \neq r_k,d}{x_{ji}^{kd}} = 1 \quad \forall k \in K, \forall d \in D^k \f$
	*
	* @param rca::Network ponteiro para rede utilizada
	* @param std::vector<std::shared_ptr<rca::Group>> grupos multicast utilizados
	*/
	void constraint3 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	/**
	* Restrição para marcar arestas que forma solução
	*
	* \f$ x_{ij}^{kd} \leq y_{ij}^{k} \quad \forall k \in K, d \in D^k, \forall (i,j) \in E \f$
	*
	* @param rca::Network ponteiro para rede utilizada
	* @param std::vector<std::shared_ptr<rca::Group>> grupos multicast utilizados
	*/
	void constraint4 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	/**
	* Garante para cada aresta que a capacidade residual é maior ou igual a
	* capacidade residual mínima.
	* 
	* @param rca::Network ponteiro para rede utilizada
	* @param std::vector<std::shared_ptr<rca::Group>> grupos multicast utilizados
	*/
	void constraint5 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	/**
	* Utilizada para implementar limites de capacidade residual ou de budget.
	*
	* Subclasses implementam.
	*
	* @param rca::Network ponteiro para rede utilizada
	* @param std::vector<std::shared_ptr<rca::Group>> grupos multicast utilizados
	*/
	void constraint6 ();
	
	/**
	* Utilizada para evitar ciclos na solução.
	*
	* @param rca::Network ponteiro para rede utilizada
	* @param std::vector<std::shared_ptr<rca::Group>> grupos multicast utilizados
	*/
	void constraint7 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	/**
	* Garante que não há fluxo de entrada em um nó fonte.
	* 
	*
	* @param rca::Network ponteiro para rede utilizada
	* @param std::vector<std::shared_ptr<rca::Group>> grupos multicast utilizados
	*/
	void constraint8 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	/**
	 * Evitar que arestas fiquem soltas na solução
	 * Proposta por elizabeth
	 *
	 * @param rca::Network ponteiro para rede utilizada
	 * @param std::vector<std::shared_ptr<rca::Group>> grupos multicast utilizados
	 */
	void out (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	/**
	* Limites necessários para as variáveis.
	*
	* @param rca::Network ponteiro para rede utilizada
	* @param std::vector<std::shared_ptr<rca::Group>> grupos multicast utilizados
	*/
	void bounds (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);

	/**
	* restrição de delay (BETA)
	*
	* @param rca::Network ponteiro para rede utilizada
	* @param std::vector<std::shared_ptr<rca::Group>> grupos multicast utilizados
	*/
	void delay (rca::Network *, 
					std::vector<std::shared_ptr<rca::Group>>&);
	
};

/**
 * Esta classe é utilizada para gerar instâncias do MMSTP-Budget.
 * Ele gerá instâncias no formato LP (Linear Program).
 * 
 * A classe herda de @MultipleMulticastCommodityLP e adiciona
 * a restrição 6 que considera um orçamento sobre a otimização.
 * 
 * Este orçamento define o valor máximo de custo da solução ao
 * otimizar a capacidade residual da rede.
 */
class MMSTPBudgetLP : protected MultipleMulticastCommodityLP
{

	
	
public:
	virtual void generate (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	/**
	* Método utilizado para configurar budget a ser utilizado durante
	* a geração do lp do multiple multicast com limite de budget.
	*
	* 
	* @param rca::Network ponteiro para rede utilizada
	* @param std::vector<std::shared_ptr<rca::Group>> grupos multicast utilizados
	* @param int budget utilizado como limite
	*/
	void generate2 (rca::Network * network,
				   std::vector<std::shared_ptr<rca::Group>>& groups, 
				   int budget)
	{
		m_budget = budget;
		generate (network, groups);
	}
	
private:
	
	/**
	 * Implementação da restrição 6 que visa a garantir que o custo
	 * da solução não ultrapasse um orçamento inicial.
	 * 
	 */
	void constraint6 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	/*variável que armazena o orçamento*/
	int m_budget;
	
};

/**
 * Esta classe é utilizada para gerar instâncias do MMSTP-congestion.
 * Ele gerá instâncias no formato LP (Linear Program).
 * 
 * A classe herda de @MultipleMulticastCommodityLP. Esta versão
 * recebe adiciona a capacidade residual oriunda da otimização do 
 * @MMSTPBudgetLP e a coloca como restrição do @MMSTPCostLP.
 * 
 * Este novo modelo, portanto, otimiza a custo com limite na 
 * capaciade residual.
 * 
 * A variável m_budget neste modelo representa a capacidade residual
 * obtida pela otimização do modelo @MMSTPBudgetLP.
 * 
 */
class MMSTPCostLP : protected MultipleMulticastCommodityLP
{

public:
	virtual void generate (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	/**
	* Método utilizado para gerar um lp considerando um limite de capacidade
	* residual mínima para ser otimizado.
	*
	* O valor de z correponde ao menor valor de capacidade residual que toda
	* aresta deve ter. 
	*
	* @param rca::Network ponteiro para rede utilizada
	* @param std::vector<std::shared_ptr<rca::Group>> grupos multicast utilizados
	* @param int capacidade residual mínima da solução.
	*/
	void generate2 (rca::Network * network,
				   std::vector<std::shared_ptr<rca::Group>>& groups, 
				   int z)
	{
		Z = z;
		generate (network, groups);
	}
	
	
private:
	/**
	 * Implementação da função para limitar a congestão das arestas
	 * Tendo em vista que o valor de limite aqui é Z(capacidade residual)
	 * e o valor a ser otimizado é o custo.
	 */
	void constraint6 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	
	int Z;
	
	
};


/**
 * Este modelo é uma implementação minha do modelo matemático
 * proposto por Lee e Cho. 
 * 
 * Este modelo otimiza a capacidade residual sujeito ao crescimento
 * das árvores multicast.
 * 
 * Para cada árvore multicast, estabelece-se um limite.
 * 
 * Este limite é o número de arestas da árvore sendo otimizada
 * individualmente.
 * 
 * Aqui eu considero o limite como sendo o custo da árvore
 * individual
 * 
 * A classe herda de @MultipleMulticastCommodityLP e adiciona
 * a restrição de Lee e Cho.
 * 
 */
class LeeAndChooModel : public MultipleMulticastCommodityLP {

public:
	
	void generate (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);

	/**
	* Método utilizado para definir o parâmetro \f$ \alpha \f$ do modelo
	* de Lee e Choo. Este parâmetro define o tamanha limite de cada árvore
	* em relação ao valor ótimo (número de arestas)
	*
	* Valor comum é: 2*OPT
	*
	* @param double valor limite de crescimento da árvore - valor comum é 2*OPT
	*/
	static void set_alpha (double alpha) {
		m_alpha = alpha;
	}
	
	/**
	* Métod utilizado para definir o valor ótimo de cada árvore para que
	* o parâmetro \f$ alpha \f$ seja aplicado.
	*
	* O aquivo consiste de um conjunto de linhas<br>
	*	<valor1><br>
	* 	<valor2><br>
	* 	<valor3><br>
	*
	* @param std::string arquivo contendo o valor ótimo de cada árvore.
	*/
	static void set_opt (std::string file) {
		
		std::ifstream in (file.c_str());
		
		if (in.good ()) {
		
			do {
				int opt;
				in >> opt;
				m_opt.push_back (opt);
			} while (!in.eof());
		}
		
	}
	
	static double m_alpha;
	
	static std::vector<int> m_opt;
	
};

#endif