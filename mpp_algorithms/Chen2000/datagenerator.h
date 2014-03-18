#include <iostream>
#include <memory>
#include <vector>
#include <fstream>
#include <sstream>

#include "group.h"
#include "network.h"
#include "reader.h"
#include "steinerReader.h"

namespace rca {

class Generator {
	
public:
	
	//constructor for MPP
	Generator (std::string);
	Generator (std::string, bool st);
	
	//generate network in a file
	const std::ostream& gen_network (std::ostream&);
	
	//generate terminal memebers in file by groups
	void gen_terminal (std::ostream&,int);
	
	void run (std::string);
	
private:
	std::shared_ptr< rca::Network > net;
	std::vector< std::shared_ptr<Group> > groups;
	
	bool st;
	
	void init ();

};

}