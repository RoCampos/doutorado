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

const ostream& Generator::gen_network (std::ostream & out) {
	
	out << "param n := ";
	out << net.get ()->getNumberNodes () << ";\n";
	
	std::set<Link> links = net.get ()->getLinks ();
	
	out << "set E :=";
	for (Link l : links) {
		out << "(" << l.getY () << "," << l.getX () << ") ";		
	}
	out << ";\n";
	
	out << "set C :=\n";
	for (Link l : links) {
		int i = l.getY();
		int j = l.getX();
		out << "\t" << l.getY () << " " << l.getX () << " " << net.get ()->getCost (i,j) << "\n";
	}
	
	out << ";\n";
	
	return out;
	
}


int main (void) {
	
	Generator g("/home/romerito/workspace/Doutorado/b30_1.brite");
	std::string file = "/home/romerito/workspace/Doutorado/b30_1";
	
	return 0;
}