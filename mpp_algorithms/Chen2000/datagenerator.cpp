#include "datagenerator.h"

using namespace std;
using namespace rca;

Generator::Generator (std::string file) {

	Reader r (file);
	
	init ();
	
	r.configNetwork ( net.get() );
	groups = r.readerGroup ();
	
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

void Generator::gen_terminal (std::ostream& out, int i) {
	
	Group * g = groups[i].get ();
	
	out << "set T :=";
	for (int i = 0; i < g->getSize (); i++) {
		out << " "<< g->getMember (i) ;
	}	
	out << ";";
}

void Generator::run (std::string file) {
	
	std::ofstream out;
	int GROUPS = groups.size ();
	
	for (int i=0; i < GROUPS; i++) {
	
		std::stringstream ss;
		ss << file << "_st_" << i << ".dat";
		
		out.open ( ss.str() );
		
		gen_network (out);
		gen_terminal (out, i);
		
		out.close ();
		
	}
	
}

int main (void) {
	
	Generator g("/home/romerito/workspace/Doutorado/b30_1.brite");
	std::string file = "/home/romerito/workspace/Doutorado/b30_1";

	g.run (file);
	
	return 0;
}