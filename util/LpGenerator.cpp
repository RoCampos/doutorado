#include <iostream>

#include "SolverDataGenerator.h"
#include "group.h"
#include "network.h"
#include "reader.h"
#include "steinerReader.h"
/**
 * Binário que gera os lps para cada instância do MPP
 * 
 * Usa a implementação de MultipleMulticastCommodityLP
 * 
 */
int main (int argv, char**argc)
{
	std::string file = argc[1];
	
	Network * net = new Network;
	Reader r (file);
	r.configNetwork ( net );
	
	/*
	Network * net = new Network(6,10);
	net->setCost (0,1,1);
	net->setCost (0,3,1);
	net->setCost (0,4,1);
	net->setCost (0,5,1);
	net->setCost (1,3,1);
	net->setCost (1,4,1);
	net->setCost (1,2,1);
	net->setCost (2,3,1);
	net->setCost (2,4,1);
	net->setCost (2,5,1);
	net->setCost (5,4,1);
	
	net->setCost (1,0,1);
	net->setCost (3,0,1);
	net->setCost (4,0,1);
	net->setCost (5,0,1);
	net->setCost (3,1,1);
	net->setCost (4,1,1);
	net->setCost (2,1,1);
	net->setCost (3,2,1);
	net->setCost (4,2,1);
	net->setCost (5,2,1);
	net->setCost (4,5,1);
	
	net->setBand (0,1,2);
	net->setBand (0,3,2);
	net->setBand (0,4,2);
	net->setBand (0,5,2);
	net->setBand (1,3,2);
	net->setBand (1,4,2);
	net->setBand (1,2,2);
	net->setBand (2,3,2);
	net->setBand (2,4,2);
	net->setBand (2,5,2);
	net->setBand (5,4,1);
	
	net->setBand (1,0,2);
	net->setBand (3,0,2);
	net->setBand (4,0,2);
	net->setBand (5,0,2);
	net->setBand (3,1,2);
	net->setBand (4,1,2);
	net->setBand (2,1,2);
	net->setBand (3,2,2);
	net->setBand (4,2,2);
	net->setBand (5,2,2);
	net->setBand (4,5,1);
	
	Group g1(0,0,1);
	Group g2(1,0,1);
	Group g3(2,2,1);
	
	g1.addMember (1);
	g1.addMember (3);
	
	g2.addMember (2);
	g2.addMember (4);
	
	
	std::vector<shared_ptr<rca::Group>> g;
	g.push_back (make_shared<rca::Group>(g1) );
	g.push_back (make_shared<rca::Group>(g2) );
	//g.push_back (make_shared<rca::Group>(g3) );
	*/
	
	std::vector<shared_ptr<rca::Group>> g = r.readerGroup ();
	
	DataGenerator<MMSTPBudgetLP> dg;
	dg.run (net, g);

	return 0;
}