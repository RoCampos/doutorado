#ifndef _MY_INDIVIDUAL_
#define _MY_INDIVIDUAL_

#include <iostream>
#include <fstream>


//ADICICIONE OS CABEÇALHOS DO SEU PROBLEMA

#include "reader.h"
#include "group.h"
#include "network.h"

using namespace rca;

namespace Info {

class Problem
{

	static Problem * problem;


private:

	Problem(std::string file, std::string conf) {
		std::cout << __FUNCTION__ << endl;
		initilization (file, conf);
	}
	~Problem() {
		std::cout << "Bullshit" << std::endl;
	}

	void initilization (std::string file, std::string obj_conf) {
		readerObj (obj_conf);

		std::vector<std::shared_ptr<rca::Group>> g;
		rca::reader::MultipleMulticastReader reader(file);
	#ifdef MODEL_REAL
		reader.configure_real_values (&(this->network),g);
	#endif	
	#ifdef MODEL_UNIT
		reader.configure_unit_values (&(this->network),g);
	#endif

		for (std::shared_ptr<rca::Group> i : g) {
			this->mgroups.push_back (*i);
		}
	}

	void readerObj (std::string obj_conf) {
		std::ifstream file_(obj_conf.c_str(), ifstream::in);	
	
		if (file_.fail()) exit (1);
		
		std::string str;
		int count = 0;
		file_ >> str;
		do {		
			count++;
			file_ >> str;
		} while (!file_.eof ());
			
		
		file_.close ();
		file_.open (obj_conf.c_str(), ifstream::in);
		this->objconf = new int[count];
		this->OBJECTIVES = count;
		for (int i=0; i < count; i++) {
			file_ >> str;
			this->objconf[i] = atoi(str.c_str());
		}
	}

public:

	static void init (std::string file, std::string conf){
		if (!problem)
			problem = new Problem (file, conf);		

		std::set<rca::Link> & links = problem->network.getLinks ();
		for (auto e : links) {			
			rca::Link l (e.getX(), e.getY(),0);
			l.setValue (problem->network.getBand(l.getX(), l.getY()));
			problem->capacity.push_back (l);
		} 

	}
	static Problem  * get_instance () {			
		return problem;		
	}

	int * objconf; //indica como manipular os objetivos
	int OBJECTIVES;
	rca::Network network;
	std::vector<rca::Group> mgroups;
	std::vector<rca::Link> capacity;
};

Problem * Problem::problem = 0;

}

#endif
