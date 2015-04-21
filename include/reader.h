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

/**
 * Esta classe reprenta o Leitor de Instâncias.
 * Ele consiste apenas de dois métodos para configuração
 * da rede e do problema.
 *
 * @date 27/09/2012
 * @author Romerito Campos
 */
class Reader {

public:

	/**
	 * Construtor recebe uma string indicando
	 * o nome da instância.
	 */
	Reader (std::string file);

	/**
	 * Este método configura um objeto do tipo Network.
	 *
	 * @see Network
	 * @param Network
	 */
	void configNetwork (Network *);
	
	std::vector<std::shared_ptr<Group>> readerGroup ();
	
	std::string getFileName () const {return m_file;}

private:
	std::string m_file;
};

class MultipleMulticastReader : public Reader {

public:
	
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
	 * 	- tk(traffic request) para todo grupo igual a 1(um)
	 *  - capacidade das arestas é igual a quantidade de grupos.
	 * 
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


typedef std::pair<std::vector<int>,std::vector<int>> stream;

typedef struct stream_bind {
	int trequest;
	stream s;	
} stream_bind;

typedef std::vector<stream_bind> stream_list;

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
	void configure_network (rca::Network &, stream_list &);
	
	/**
	 * Method to read the streams.
	 * Receives two parameters: a vector to store the sources
	 * and a vector to store the destination nodes of the stream.
	 *
	 * @param std::vector<int> sources
	 * @param std::vector<int> destinations
	 */
	void configure_streams (stream_list &);
	
	int get_number_nodes () {return m_nodes;}
	int get_number_edges () {return m_edges;}
	int get_number_streams () {return m_streams;}
	
private:
	
	int m_nodes;
	int m_edges;
	int m_streams;
	
};

#endif /* READER_H_ */
