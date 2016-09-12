/*
 * steinerReader.h
 *
 *  Created on: 04/11/2012
 *      Author: romerito
 */

#ifndef STEINERREADER_H_
#define STEINERREADER_H_

#include <fstream>
#include <string>
#include <vector>
#include <iostream>

#include "network.h"
#include "link.h"
#include "group.h"

namespace rca {

/**
* Classe implementa um parser para instância do 
* problema da árvore de Steiner em grafos.
* 
* O parser faz a leitura das instâncias com base na 
* seguinte representação: http://steinlib.zib.de/steinlib.php
*
* 
* @author Romerito C. Andrade
* @version 0.5.0
*/
class SteinerReader {

public:

	/**
	* Construtor da classe SteinerReader.
	*
	* Recebe como parâmetro uma string indicado o arquivo
	* que armazena uma instância do problema da árvore de Steiner em Grafo.
	*
	* @param std::string caminho para instância do problema da árvore de Steiner
	*/
	SteinerReader(std::string);

	/**
	* Destrutor do classe.
	*
	*/
	~SteinerReader();

	/**
	* Método utilizado para configurar o grafo com
	* base nas informações da instâncias de Steiner.
	*
	* @param rca::Network ponteiro para objeto do tipo rca::Network
	*/
	void configNetwork (Network *network);

	/**
	* Método que retorna os nós terminais de uma instância dó
	* do problema da árvore de Steiner como um objeto do tipo
	* rca::Group.
	* 
	* @return rca::Group objeto grupo que armazena os nós terminais.
	*/
	Group terminalsAsGroup ();

private:
	int m_numberVertex;
	int m_numberEdges;
	int m_numberTerminais;
	std::vector<int> m_terminais;
	std::vector<std::vector<int> > m_edges;
	std::vector<Link> m_edges_set;

};


void get_steiner_info (rca::Network&, Group&, std::string file);

} /* namespace rca */
#endif /* STEINERREADER_H_ */
