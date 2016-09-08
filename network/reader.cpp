/*
 * reader.cpp
 *
 *  Created on: 12/09/2012
 *      Author: romerito
 */
//namespace rca {

#include "reader.h"

using namespace rca::reader;

Reader::Reader(std::string file) {

	m_file = file;
}

void Reader::configNetwork(Network * network) {

	ifstream file(m_file.c_str(), ifstream::in);

	if (file.fail()) {
		cout << "Invalid Input Try Again, Cannot Read the Network File!\n";
		exit (1);
	}

	int m_nodes = 0;
	int m_edges = 0;

	std::string str;
	for (int i=0 ;i < 3;i++) {
		file >> str;
	}

	m_nodes = atoi (str.c_str()); //reading m_nodes
	file >> str; //jump
	file >> str; //reading edges
	m_edges = atoi (str.c_str());

	network->init(m_nodes,m_edges);

	int i = 0;
	for (int i=0 ;i < 4 + m_nodes + 3;i++)
		getline (file, str);

	//reading the edges
	for (i=0; i < m_edges; i++) {
		file >> str; // id
		file >> str; //from
		int from = atoi (str.c_str());
		file >> str; //to
		int to = atoi (str.c_str());
		file >> str; //cost
		double d = atof (str.c_str());
		network->setCost(from,to,d);
		network->setCost(to,from,d);
		file >> str;
		file >> str; //band
		d = atof (str.c_str());
		d+=20;
		network->setBand(from,to,d);
		network->setBand(to,from,d);

		rca::Link link(from,to,d);
		network->insertLink(link);
		
		network->addAdjacentVertex(from, to);
		network->addAdjacentVertex(to, from);

		getline (file, str);
	}

	file.close();

}

std::vector<std::shared_ptr<Group>> Reader:: readerGroup () {
	
	std::vector<std::shared_ptr<Group>> list_of_group;
	
	ifstream file(m_file.c_str(), ifstream::in);
	if (file.fail()) {
		cout << "Invalid Input Try Again, Cannot Read the Network File!\n";
		exit (1);
	}
	
	std::string str;
	for (int i=0; i < 4; i++)
		getline (file, str);
	file >> str;
	
	while (str.compare("Groups:")) {
		file >> str;
	}
	
	//configuring the number of groups
	int m_groups = 0;
	file >> str;
	file >> m_groups;
	file >> str;
	
		getline (file,str);
	//reading groups
	for (int i=0; i < m_groups; i++) {

		//deleting unnecessary informations
		for (int j=0; j < 7; j++)
			file >> str;

		//getting the traffic
		double t_k = 0;
		file >> t_k;
		file >> str;
		file >> str; file >> str;

		//geting the source
		int source = 0;
		file >> source;
		file >> str; file >> str; file >> str;

		int size = 0;
		file >> size;
		file >> str;

		//creating group with id=i
		shared_ptr<Group> ptr (new Group(i,source, t_k));
		//Group * group = new Group(i,source,t_k);
		

		//getting the members
		for (int k = 0; k < size; k++) {
			int member = 0 ;
			file >> member;
			//group->addMember (member);
			ptr.get()->addMember (member);
		}
		
		list_of_group.push_back (ptr);

		getline (file, str);
	}
	
	file.close ();
	
	return list_of_group;
	
}


void rca::reader::get_problem_informations (std::string const& file, 
	rca::Network &net, std::vector<rca::Group> & mgroups)
{

	// reader
	MultipleMulticastReader reader (file);

	// temp struct for groups
	std::vector<std::shared_ptr<rca::Group>> _mgroups;

#ifdef MODEL_REAL
	reader.configure_real_values (&net,_mgroups);
#endif 

#ifdef MODEL_UNIT
	reader.configure_unit_values (&net,_mgroups);
#endif

	for (auto && g : _mgroups) {
		//getting from shared_ptr to Group Instance.
		mgroups.push_back ( *g );
	}

}

//} /*namespace rca*/
