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

class SteinerReader {

public:
	SteinerReader(std::string);
	~SteinerReader();

	void configNetwork (Network *network);
	Group terminalsAsGroup ();

private:
	int m_numberVertex;
	int m_numberEdges;
	int m_numberTerminais;
	std::vector<int> m_terminais;
	std::vector<std::vector<int> > m_edges;
	std::vector<Link> m_edges_set;

};

} /* namespace rca */
#endif /* STEINERREADER_H_ */
