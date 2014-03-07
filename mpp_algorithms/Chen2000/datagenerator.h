#include <iostream>
#include <memory>
#include <vector>
#include <fstream>

#include "group.h"
#include "network.h"
#include "reader.h"

namespace rca {

class Generator {
	
public:
	Generator (std::string);
	
	const std::ostream& gen_network (std::ostream&);
	void gen_terminal ();
	
	void run ();
	
private:
	std::shared_ptr< rca::Network > net;
	std::vector< std::shared_ptr<Group> > groups;
	
	void init ();

};

}