#include <iostream>
#include <memory>
#include <vector>

#include "group.h"
#include "network.h"

namespace rca {

class Generator {
	
public:
	Generator (std::string);
	
	void gen_network ();
	void gen_terminal ();
	
	
private:
	std::shared_ptr< rca::Network > net;
	std::vector< std::shared_ptr<Group> > groups;

};

}