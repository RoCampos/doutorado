#ifndef _INSTANCE_GENERATOR_H_
#define _INSTANCE_GENERATOR_H_

#include <cstdio>

#include <iostream>
#include <fstream>
#include <ios>
#include <string>
#include <vector>
#include <numeric>

#include "myrandom.h"


#define PERCENTUAL 0
#define FIXED 1

typedef rca::myrandom<std::mt19937, std::uniform_int_distribution<int>, int> RandomInt;	
typedef rca::myrandom<std::mt19937, std::uniform_int_distribution<float>, float> RandomFloat;

// enum InstanceModel { PERCENTUAL = 0, FIXED = 1};

/**
 * Classe usada para gerar informações de grupos multicast em instâncias.
 * É possível gerar instâncias levando em consideração a variação no tamanho
 * dos grupos multicast. Assim como, no valor de traffic request.
 *
 * O método set_mode permite definir se os valores passados aos métodos
 * set_capacity, set_traffic, set_num_groups são valores fixos ou percentuais.
 *
 *
 * É possível ter diferentes combinações dos parâmetros que são utilizados
 * para gerar as características dos grupos multicast e colocá-las em uma
 * instância.
 * Por exemplo: 
 * 	- Número de grupos
 * 	- Número de nós por grupos
 * 	- Capacidade máxima e mínima das arestas 
 * 	- Tráfego requerido por cada grupo multicast
 * 
 * O gerador utiliza a api rca::myrandom para geração de valores aleatórios.
 * Esta api utiliza o gerador mersenne twister.
 *
 * @author Romerito Campos.
 * @date 06/08/2015 - junho
 */
class InstanceGenerator {

public:
	
	/**
	* Construtor da classe.
	*
	* O consstrutor da classe recebe como entrada uma string indicando
	* o aquivo de entrada. Este arquivo é uma topologia BRITE.
	*
	* @param std::string arquivo de entrada
	*/
	InstanceGenerator (std::string file) {
		
		this->m_file_name = file;
		this->m_seed = time(NULL);
		m_randomInt = RandomInt (this->m_seed, 1, 5);
		
	};
	
	/**
	* Método que executa a geração das características da instância.
	*
	*/
	void generator ();
	
	/**
	 * Define o modo que será utilizado para gerar um instância
	 * Quando um parâmetro recebe 0, significa que ele usará
	 * o percentual em relaçaõ aos valores passados.
	 * 
	 * Quando um parâmetro recebe 1, significa que os valores
	 * de tk e tamanho de grupo serão fixos.
	 *
	 * @param bool g_mode - true(1) indica valores fixos, o contrário valores pecentuais
	 * @param bool cap_mode - true(1) indica valores fixos, o contrário valores pecentuais
	 * @param bool tk_mode - true(1) indica valores fixos, o contrário valores pecentuais
	 * 
	 */
	void set_mode (bool g_mode, bool cap_mode, bool tk_mode);
	
	/**
	* Método para definir o número de grupos.
	* @param int número de grupos.
	*/
	void set_num_groups (int num)
	{
		this->m_num_groups = num;
	}
	
	/**
	* Método para definir o número de nós da rede
	* @param int número de nós
	*/
	void set_num_nodes (int V)
	{
		this->m_num_nodes = V;
	}
	
	/**
	* Método que define o tamanho dos grupos
	* @param double tamanho mínimo do grupo
	* @param double tamanho máximo do grupo
	*/
	void set_group_size (double perc_min, double perc_max)
	{
		this->m_g_min = perc_min;
		this->m_g_max = perc_max;
	}
	
	/**
	* Método que define a capacidade das arestas 
	* 
	* @param double capacidade mínima.
	* @param double capacidade máxima.
	*/
	void set_capacity (double perc_min, double perc_max)
	{
		this->m_c_min = perc_min;
		this->m_c_max = perc_max;
	}
	
	/**
	* Método utilizado para definir o consumo máximo e mínimo
	* por cada grupo multicast.
	* 
	* 
	*
	* @param double consumo mínimo.
	* @param double consumo máximo.
	*/
	void set_traffic (double perc_min, double perc_max)
	{
		this->m_tk_min = perc_min;
		this->m_tk_max = perc_max;
	}
	
private:
	
	/**
	 * Create a temporaly list of vertex to be used at 
	 * building of groups
	 * 
	 */
	std::vector<int> create_list () {		
		std::vector<int> list(m_num_nodes);
		std::iota (list.begin(), list.end(), 0);		
		return list;
	}
	
	/**
	 * Gera informações de tráfego
	 */
	void generate_traffic ( std::ostream & out );
	
	/**
	 * Gera informações de grupo
	 * 
	 */
	void generate_group_info ( std::ostream & out);
	
	/**
	 * Gera inforamções dos membros de um grupo
	 * 
	 */
	void generate_members ( std::ostream & out, 
							std::vector<int>&, 
							int group_size);
	
private:
	
	std::string m_file_name;
	int m_num_nodes;
	int m_num_groups;
	
	int m_group_mode; //1 - valores fixos, 0 - percentual
	int m_capacity_mode; // 1 - valores fixos, 0 - percentual
	int m_traffic_mode; // 1 - valores fixos, 0 - percentual
	
	double m_tk_min, m_tk_max; //limites dos valores utilizados para tk
	double m_c_min, m_c_max; //limites dos valores utilizados para capacidade
	double m_g_min, m_g_max; //limites dos valores utilizados para tamanho de grupos
	
	int m_seed;
	
	RandomInt m_randomInt;
	
};
	

#endif /*_INSTANCE_GENERATOR_H_*/