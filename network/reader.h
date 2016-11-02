#ifndef READER_H_
#define READER_H_

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <cmath>
#include <cstring>
#include <boost/tokenizer.hpp>

#include "network.h"
#include "link.h"
#include "group.h"
#include "matrix2d.h"

#include <memory>

using rca::Network;
using rca::Group;
using namespace std;
using dl::matrix2d;

namespace rca {

	namespace reader {

/**
 * Esta classe reprenta o Leitor de Instâncias.
 * Ele consiste apenas de dois métodos para configuração
 * da rede e do problema.
 *
 * @date 27/09/2012
 * @author Romerito Campos
 * @version 0.5.0
 */
class Reader {

public:

	/**
	 * Construtor recebe uma string indicando
	 * o nome do arquivo instância.
	 *
	 * @param std::string
	 */
	Reader (std::string file);

	/**
	 * Este método configura um objeto do tipo Network.
	 * 
	 * O Objetivo network é criado e passado para o método
	 * configNetwork. Este método vai chamar @see init() de
	 * Network e configurar o número de arestas e vértices.
	 * Após, vai adicionar todas as arestas da instância para
	 * o objeto network.
	 *
	 * @see Network
	 * @param Network
	 */
	void configNetwork (Network *);
	

	/**
	* Método utilizado para extrair os grupos multicast
	* do arquivo que representa uma instância.
	* 
	* 
	* @return std::vector<std::shared_ptr<rca::Group> >
	*/
	std::vector<std::shared_ptr<Group>> readerGroup ();
	

	/**
	* Método utilizado para retornar o nome da instância que
	* está sendo manipulada pelo reader.
	*
	*
	* @return std::string
	*/
	std::string getFileName () const {return m_file;}

private:
	std::string m_file;
};

/**
* Esta classe implementa um parser para o problema de
* de roteamento multicast com múltiplos grupos.

* Os grupos multicast e a rede que serão utilizadas como entrada
* para resolução do caso de teste são configurados de duas formas:

* Primeiro, utilizando valores reais da instâncias, onde se tem requerimento
* de tráfego com valor aleatório e a capacidade das arestas como 
* valores tambéma aleatórios.
* Segundo, todas as arestas tem capacidade igual ao número de grupos
* multicast presente na instância. O consumo de cada grupo é igual a 1.
* 
* @author Romerito C. Andrade
* @version 0.5.0
*/
class MultipleMulticastReader : public Reader {

public:
	

	/**
	* Construtor da class MultipleMulticastReader
	* faz chamada ao construtor da classe base @see Reader.
	*
	* 
	*
	* @param std::string file
	*/
	MultipleMulticastReader (std::string file) : Reader(file)
	{
		
	}
	
	/**
	 * Este método configura os grupos e a rede de
	 * acordo com os valores da instância.
	 * 
	 * TRequest igual o valor definido na instância
	 * Capacidade das arestas igual o valor das instâncias.
	 */
	void configure_real_values (rca::Network* net, 								
								std::vector<std::shared_ptr<Group>>& groups) 
	{
		this->configNetwork (net);
		std::vector<std::shared_ptr<Group>> tmp = this->readerGroup ();
		for (std::shared_ptr<Group> g : tmp) {
			groups.push_back (g);
		}		
	}
	
	/**
	 * Este método configura os grupos e o objeto Network considerando as 
	 * seguintes condições:
	 * 	- tk(traffic request) para todo grupo igual a 1(um).
	 *  - capacidade das arestas é igual a quantidade de grupos.
	 * 
	 * @param Network
	 * @param std::vector<std::shared_ptr<Group> >
	 */
	void configure_unit_values (rca::Network* net, 								
								std::vector<std::shared_ptr<Group>>& groups) {
	
		this->configure_real_values(net, groups);
		this->setBand (net, groups.size ());
		this->setTRequest (groups);
		
	}
	
private:
	
	/**
	 * Método auxiliar que define a capacida das arestas como band
	 * 
	 * @param band
	 * @param rca::Network
	 * @return void
	 */
	void setBand (rca::Network * net, int band) {
		int NODES = net->getNumberNodes ();
		for (int i = 0; i < NODES;i++) {
			for (int j = 0; j < NODES;j++) {	
				if (net->getBand (i,j) > 0) {
					net->setBand (i,j, band);
				}
			}
		}	
	}
	
	/**
	 * Método auxiliar que define o requerimento de tráfego de cada
	 * grupo como sendo 1.
	 * 
	 * @param std::vector<std::shared_ptr<Group>>
	 */
	void setTRequest (std::vector<std::shared_ptr<Group>>& groups) {
		
		int SIZE = groups.size ();
		for (int i=0; i < SIZE; i++) {
			groups[i]->setTrequest (1);
		}
		
	}
	
};

//pair of sources(first) and mambers(second)
typedef std::pair<std::vector<int>,std::vector<int>> stream;

typedef struct stream_bind {
	int trequest;
	stream s;
	
	std::vector<int> & get_sources () {return s.first;}
	std::vector<int> & get_members () {return s.second;}
	
} stream_bind;

typedef std::vector<stream_bind> stream_list_t;

/**
* Esta classe é utilizada como parser das instâncias 
* criadas por Youh-Chen e cedidas para minha pesquisa.
*
* AS instâncias possuem uma rede e um conjunto de grupos 
* multicast. 
*
* Cada grupo multicast pode ser atendido por mais de uma fonte,
* multiplos streams
*
* @author Romerito C. Andrade
* @version 0.5.0
*/
class YuhChenReader : public Reader
{

public:
	/**
	 * Default constructor. Receives a instance name.
	 * Initializate the Reader constructor.
	 * 
	 * @param std::string file name
	 */
	YuhChenReader (std::string instance) : Reader(instance) {}
	
	/**
	 * Method used to configure network information.
	 * 
	 * @param rca::Network
	 */
	void configure_network (rca::Network &, stream_list_t &);

	void configure_network (rca::Network &, std::vector<rca::Group>&);
		
	int get_number_nodes () {return m_nodes;}
	int get_number_edges () {return m_edges;}
	int get_number_streams () {return m_streams;}
	
private:
	
	int m_nodes;
	int m_edges;
	int m_streams;
	
};


void get_problem_informations (
	std::string const& file, 
	rca::Network &net, 
	std::vector<rca::Group> &mgroups);

	} //end of namespace reader
} //end of namespace rca

#endif /* READER_H_ */