#include "datagenerator.h"

using namespace std;
using namespace rca;

Generator::Generator (std::string file) {

	Reader r (file);
	
	init ();
	
	r.configNetwork ( net.get() );
	
}

void Generator::init () {
	
	Network _net;
	net = make_shared<Network> (_net);
	
}

void Generator::gen_network () {
	
	std::set<Link> links = net.get ()->getLinks ();
	
	for (Link l : links) {
		cout << l << endl;
	}
	
}


int main (void) {
	
	Generator g("/worksace/Doutorado/b30_1.brite");
	
	return 0;
}