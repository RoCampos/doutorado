#ifndef READER_H_
#define READER_H_

#include <iostream>
#include <cstdlib>
#include <string>
#include <fstream>
#include <cmath>
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


private:
	std::string m_file;
};

#endif /* READER_H_ */
