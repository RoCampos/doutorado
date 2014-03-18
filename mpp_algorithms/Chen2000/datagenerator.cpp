#include "datagenerator.h"

using namespace std;
using namespace rca;

Generator::Generator (std::string file) {

	Reader r (file);
	
	init ();
	
	r.configNetwork ( net.get() );
	groups = r.readerGroup ();
	
}

Generator::Generator (std::string file, bool st) {
	
	if (st) {
	
		SteinerReader str(file);
		init ();
		str.configNetwork ( net.get() );
		Group g = str.terminalsAsGroup ();
		groups.push_back ( make_shared<Group> (g) );
		
	}	
}

void Generator::init () {
	
	Network _net;
	net = make_shared<Network> (_net);
	
}

const ostream& Generator::gen_network (std::ostream & out) {
	
	out << "param n := ";
	out << net.get ()->getNumberNodes () << ";\n";
	
	std::set<Link> links = net.get ()->getLinks ();
	int NODES = net.get ()->getNumberNodes ();
	out << "set E :=";
	//for (Link l : links) {		
		//out << "(" << l.getY () + 1 << "," << l.getX () + 1 << ") ";		
	//}
	for (int i = 0; i < NODES; i++) {			
		for (int j = i; j < NODES; j++) {
			
			if ( net.get ()->getCost ( i, j) > 0.0 ) {
				out << "(" << i + 1 << "," << j + 1 << ") ";
			}
		}
	}	
	out << ";\n";
	
	out << "param c :\n";	
	for (int i = 0; i < NODES; i++) {
		out << i + 1<< "\t";
	}
	out << ":=\n";
	
	for (int i = 0; i < NODES; i++) {
		out << i +1 << "\t";
		for (int j = 0; j < NODES; j++) {
			out <<net.get ()->getCost ( i, j) << "\t";
		}
		out << "\n";
	}
	
	out << ";\n";

	
	return out;
	
}

void Generator::gen_terminal (std::ostream& out, int i) {
	
	Group * g = groups[i].get ();
	
	out << "set T :=";
	
	if ( Generator::st ) {
		out << " " << g->getSource ();
	}
	
	for (int i = 0; i < g->getSize (); i++) {		
		
		if (Generator::st) {
			out << " "<< g->getMember (i);
		} else {
			out << " "<< g->getMember (i) + 1;
		}
	
	}
	
	out << ";\n";
	
	if ( Generator::st ) {
		out << "param r := " << g->getSource () << ";\n";
	} else {
		out << "param r := " << g->getMember(0) + 1<< ";\n";
	}
	
	
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

int main (int argc, char **argv) {
	
	std::string str (argv[1]);
	Generator g(str,1);
	std::string file = str;

	g.run (file);
	
	return 0;
}