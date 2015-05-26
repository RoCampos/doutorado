#ifndef _SOLVER_DATA_GENERATOR_H_
#define _SOLVER_DATA_GENERATOR_H_

#include <iostream>
#include <ostream>
#include <sstream>
#include <memory>

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
 */
class MultiCommidityFormulation 
{
	
public:
	
	void generate (rca::Network * network, rca::Group *g) ;
	
};

/** 
 * @MultipleMulticastCommodityFormulation usado para gerar dados no formado
 * dat para o problema Multiple Multicas Packing que maximiza capacidade residual.
 * Esta versão não possui orçamento. Os arquivos dat deve ser utilizados em
 * conjunto com o modelo mmmstp.mod.
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
	
	void constraint1 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	void constraint2 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	void constraint3 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	void constraint4 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	void constraint5 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	void constraint6 ();
	
	void constraint7 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	void constraint8 (rca::Network *,
				   std::vector<std::shared_ptr<rca::Group>>&);
	
	void bounds (rca::Network *,
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

#endif