#include "reader.h"

void YuhChenReader::configure_network (rca::Network & network, stream_list & list)
{

	std::ifstream file ( getFileName().c_str() );
	
	if (file.fail()) {
		cout << "Invalid Input Try Again, Cannot Read the Network File!\n";
		exit (1);
	}
	
	//int NODES, EDGES, STREAMS;
	
	file >> this->m_nodes;
	file >> this->m_edges;
	file >> this->m_streams;

	network.init (this->m_nodes, this->m_edges);
	
	//reading the edges
	for (int i=0; i < this->m_edges; i++) {
		int x, y, w;
		file >> x >> y >> w;

		network.setCost (x,y,w);
		network.setCost (y,w,w);
		network.setBand (x,y,w);
		network.setBand (y,x,w);
	}
	
	typedef boost::tokenizer<boost::char_separator<char>,
			std::string::const_iterator, std::string> tokenizer;
	
	std::string line;	
	boost::char_separator<char> sep;	
	getline (file, line);
	getline (file, line);
	do {
		
 		tokenizer tokens (line);
 		
		std::vector<int> sources;
		std::vector<int> members;
		
		int i=0;		
		int trequest;
		int members_size;
 		for (auto it : tokens) {
 			std::string str = it;			
			if (i==0) {
				trequest = atoi (str.c_str ());
			} else if (i == 1) {
				members_size = atoi (str.c_str ()); 
			} else if (i < (members_size + 2) ) {
				sources.push_back ( atoi ( str.c_str () ) );
			} else if (i >= (members_size + 2) ) {
				members.push_back ( atoi ( str.c_str () ) );
			}
 			
 			i++;
 		}
 		
 		stream_bind sbind;
		sbind.trequest = trequest;
		sbind.s = std::make_pair(sources, members);
		
		list.push_back (sbind);	 		
	
	} while ( getline (file, line) );
}

void YuhChenReader::configure_streams (stream_list & list)
{
	
	
	
}
