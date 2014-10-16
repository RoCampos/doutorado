#include <iostream>
#include <string>
#include <fstream>

#include "stalgorithm.h"
#include "kmb.h"
#include "network.h"
#include "steinertree.h"
#include "reader.h"

void processing (SteinerTree & ref, std::string & file);

int main (int argc, char**argv) {

	std::string file = argv[1];
	Reader r(file);	
	//creating the network object
	rca::Network network;	
	//configuring the network object
	r.configNetwork (&network);	
	std::vector<std::shared_ptr<rca::Group>> m_groups = r.readerGroup ();
	
	STAlgorithm<KMB, rca::Network, SteinerTree, rca::Group> algo;

	//rca::Group g = *m_groups[1].get();
	
	std::string output (argv[2]);
	
	for (unsigned i=0;  i < m_groups.size (); i++) {
	
		SteinerTree st(network.getNumberNodes());
		algo.build (network, st, *m_groups[i].get());
		//st.xdotFormat ();
		processing (st, output);
	
	}
	
#ifdef DEBUG1
	std::cout << st.getCost () << std::endl;
    std::string f ="/home/romeritocampos/workspace/Doutorado/";
    f = f+"/saida.xdot";
    std::cout << f << std::endl;
    st.xdotToFile (f);
    std::string cmd ("xdot");
    cmd += " " + f;
	system (cmd.c_str ());
	getchar ();
#endif
	
	return 0;
}

void processing (SteinerTree & ref, std::string & file){

	std::ofstream output(file.c_str(), std::ofstream::out | std::ofstream::app);
	
	Edge * perc = ref.listEdge.first ();
	
	while (perc != NULL) {
	
		output << perc->i + 1 << " - " << perc->j + 1 <<":1;"<< std::endl;
		
		perc = perc->next;
	}
	
	output << std::endl;
	output.close ();
	
}