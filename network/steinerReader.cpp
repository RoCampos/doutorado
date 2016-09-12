/*
 * steinerReader.cpp
 *
 *  Created on: 04/11/2012
 *      Author: romerito
 */

#include "steinerReader.h"

namespace rca {

using namespace std;

SteinerReader::SteinerReader(std::string file) {

	ifstream file_(file.c_str(), ifstream::in);

	if (file_.fail()) {
		cout << "Fail in Reading Steiner Tree File\n";
		exit(1);
	}

	string str;
	while (file_.good()) {
		file_ >> str;
		if (str.compare("Nodes") == 0) {
			file_ >> m_numberVertex;
		}

		if (str.compare("Edges") == 0) {
			file_ >> m_numberEdges;
		}

		if (str.compare("Terminals") == 0){
			file_ >> str;
			file_ >> m_numberTerminais;
		}
	}

	m_edges = vector<vector<int> >(m_numberVertex);
	for (int i = 0; i < m_numberVertex; ++i) {
		m_edges[i] = vector<int> (m_numberVertex,0);
	}

	file_.close(); //fecha

	file_.open(file.c_str(),ifstream::in); //reabre

	while (file_.good()) {
		file_ >> str;

		if (str.compare("E")==0) {
			int edge1, edge2, weigth;
			file_ >> edge1;
			file_ >> edge2;
			file_ >> weigth;

			edge1--;
			edge2--;

			Link link(edge1,edge2,weigth);
			m_edges_set.push_back(link);

			m_edges[edge1][edge2] = weigth;
			m_edges[edge2][edge1] = weigth;

			
			
		}

		if (str.compare("T") == 0) {
			int t;
			file_ >> t;
			
			//modification to see the terminals with -1
			m_terminais.push_back( (t-1) );
		}
	}

}

SteinerReader::~SteinerReader() {
	// TODO Auto-generated destructor stub
}

void SteinerReader::configNetwork(Network* network) {

	network->init(m_numberVertex,m_numberEdges);

	std::vector<Link>::iterator it = m_edges_set.begin ();
	for (; it != m_edges_set.end(); ++it) {

		Link link(it->getX(),it->getY(), it->getValue());

		network->setCost(link.getX(),link.getY(),link.getValue());
		network->setCost(link.getY(),link.getX(),link.getValue());

		network->insertLink(link);
		
		//adding adjacent vertex support
		network->addAdjacentVertex(link.getX(), link.getY());
		network->addAdjacentVertex(link.getY(), link.getX());

	}

}

Group SteinerReader::terminalsAsGroup() {

	Group group(0,m_terminais[0],0);

	for (unsigned i = 1; i < m_terminais.size (); ++i) {
		group.addMember(m_terminais[i]);
	}

	return group;
}


void get_steiner_info (
	rca::Network& network, 
	Group& group, 
	std::string file)
{

	SteinerReader sr (file);
	sr.configNetwork (&network);
	group = sr.terminalsAsGroup ();

}


} /* namespace rca */
