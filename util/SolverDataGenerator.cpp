#include "SolverDataGenerator.h"

int MultiCommidityFormulation::m_tree = 0;

void MultiCommidityFormulation::generate (rca::Network * network, 
										  rca::Group *g)
{
	
	std::stringstream ss;
	ss << "tree_" + std::to_string(m_tree) + ".stp";
	
	m_tree++;
	
	std::ofstream out( ss.str().c_str(), std::ofstream::out);
	
	out << "set V := ";
	for (int i=0;i < network->getNumberNodes (); i++) {
		out << i + 1 << " ";
	}
	out << ";\n";

	out << "\nset LINKS :=";
	for (int i=0; i < network->getNumberNodes (); i++) {
		for (int j=0; j < i; j++) {
			
			if (network->getCost (i,j) > 0.0) {
				out <<"\n\t(" <<  i+1 << "," << j+1 << ")";
				out <<"\n\t(" <<  j+1 << "," << i+1 << ")";
			}
			
		}	
	}
	out << " ;\n";
	
	out << "\nset T := ";
	for (int i=0; i < g->getSize (); i++) {
		out << " " << g->getMember (i) + 1;
	}
	out << " ;\n";
	
	out << "param r := " << g->getSource ()+1 << ";\n";
	
	out << "param : cost delay:=";
	for (int i=0; i < network->getNumberNodes (); i++) {
		for (int j=0; j < i; j++) {
			
			if (network->getCost (i,j) > 0.0) {
				out <<"\n\t" <<  i+1 << "\t" << j+1;
				out <<"\t" << (int)network->getCost (i,j);
				out <<"\t" << (int)network->getBand (i,j);
				
				out <<"\n\t" <<  j+1 << "\t" << i+1;
				out <<"\t" << (int)network->getCost (i,j);
				out <<"\t" << (int)network->getBand (i,j);
			}			

		}	
	}
	out << ";\n";
	
	out.close ();
}

void MultipleMulticastCommodityFormulation::generate (rca::Network * network, 
													  std::vector<std::shared_ptr<rca::Group>> & groups)
{
	//gerando o conjunto de vértices
	std::cout << "set VERTEX := ";
	for (int i=0;i < network->getNumberNodes (); i++) {
		std::cout << i + 1 << " ";
	}
	std::cout << ";\n";
	
	//gerando o conjunto de arestas
	std::cout << "\nset LINKS :=";
	for (int i=0; i < network->getNumberNodes (); i++) {
		for (int j=0; j < i; j++) {
			
			if (network->getCost (i,j) > 0.0) {
				std::cout <<"\n\t(" <<  i+1 << "," << j+1 << ")";
				std::cout <<"\n\t(" <<  j+1 << "," << i+1 << ")";
			}
			
		}	
	}
	std::cout << " ;\n";
	
	//gerando param de groups
	std::cout << "set GROUPS := ";
	for (unsigned int i=0; i < groups.size (); i++) {
		std::cout << " " << i + 1;
	}
	std::cout << ";\n";
	
	//gerando param de raízes
	std::cout << "param Mroot := \n";
	for (unsigned int i=0; i < groups.size (); i++) {
		std::cout << "\t" << i+1 << " " << groups[i]->getSource () + 1<<"\n";
	}
	std::cout << ";\n";
	
	std::cout << "set MEMBER := \n";
	for (unsigned int i=0; i < groups.size (); i++) {
		
		std::vector<int> members = groups[i]->getMembers ();
		for (unsigned int j = 0; j < members.size (); j++) {
			
			std::cout << "\t" << i+1 << " " << (members[j] + 1) << "\n"; 
			
		}
		
	}
	std::cout << ";\n";
	
	//gerando parametros de cost e delay
	std::cout << "param : cost cap:=";
	for (int i=0; i < network->getNumberNodes (); i++) {
		for (int j=0; j < i; j++) {
			
			if (network->getCost (i,j) > 0.0) {
				std::cout <<"\n\t" <<  i+1 << "\t" << j+1;
				std::cout <<"\t" << (int)network->getCost (i,j);
				std::cout <<"\t" << (int)network->getBand (i,j);
				
				std::cout <<"\n\t" <<  j+1 << "\t" << i+1;
				std::cout <<"\t" << (int)network->getCost (i,j);
				std::cout <<"\t" << (int)network->getBand (i,j);
			}			

		}	
	}
	std::cout << ";\n";

	std::cout << "param traffic := \n";
	for (unsigned int i=0; i < groups.size (); i++) {
		
		std::cout << "\t" << i+1 << " " << (int)groups[i]->getTrequest () << "\n";
		
	}
	std::cout << ";\n";
	
}


/*****/
///CLASS MultipleMulticastCommodityLP

void MultipleMulticastCommodityLP::generate (rca::Network * network, 
							std::vector<std::shared_ptr<rca::Group>> & groups)
{
	std::cout << "Maximize\n objective: + Z\n\nSubject To\n";
	
	constraint1 (network, groups);
	constraint2 (network, groups);
	constraint3 (network, groups);
	constraint4 (network, groups);
	constraint5 (network, groups);
	//void constraint6 ();
	constraint7 (network, groups);
	constraint8 (network, groups);
	
	bounds (network, groups);
	
}

void MultipleMulticastCommodityLP::constraint1 (rca::Network * net, 
							std::vector<std::shared_ptr<rca::Group>> & groups)
{
	//r1{k in GROUPS, d in MGROUPS[k]}:
	//	sum {(i,Mroot[k]) in  LINKS} x[i, Mroot[k], k, d] -
	//	sum {(Mroot[k], i) in  LINKS} x[Mroot[k], i, k, d] = -1;

	int GROUPS = groups.size ();
	int NODES = net->getNumberNodes ();
	
	//{k in groups,
	for (int k=0; k < GROUPS; k++) {	
		
		std::vector<int> g_i = groups[k]->getMembers ();
		
		std::sort (g_i.begin(), g_i.end());
		
		int source = groups[k]->getSource ();
		// d in MGROUPS[K]}
		for (int i=0; i < (int)g_i.size (); i++) {
			
			printf (" r1(%d,%d): ", k+1, g_i[i] +1);
			
			//SUM x(j,r,k,d)
			for (int j=(NODES-1); j >=0; j--) {
				if (net->getCost (source, j) > 0) {
					printf ("+ x(%d,%d,%d,%d) ",j+1, source+1,k+1, g_i[i] + 1);
				}
			}			
			printf ("\n ");
			// - sum x(r,j,k,d)
			for (int j=NODES-1; j >=0; j--) {
				if (net->getCost (source, j) > 0) {
					printf ("- x(%d,%d,%d,%d) ",source+1,j+1,k+1, g_i[i] + 1);
				}
			}
			//= 1
			printf ("= -1\n");
		}
	}
	
}

void MultipleMulticastCommodityLP::constraint2 (rca::Network * net, 
							std::vector<std::shared_ptr<rca::Group>> & groups)
{

	//r2{k in GROUPS, d in MGROUPS[k]}:
		//sum { (i,d) in LINKS} x[i, d, k, d] -
		//sum { (d,i) in LINKS} x[d, i, k, d] = 1;

	int GROUPS = groups.size ();
	int NODES = net->getNumberNodes ();
	
	//{k in groups,
	for (int k=0; k < GROUPS; k++) {	
		
		std::vector<int> g_i = groups[k]->getMembers ();
		
		std::sort (g_i.begin (), g_i.end());
		//int source = groups[k]->getSource ();
		// d in MGROUPS[K]}
		for (int i=0; i < (int)g_i.size (); i++) {
			
			printf (" r2(%d,%d): ", k+1, g_i[i] +1);
			
			//SUM x(j,r,k,d)
			for (int j=NODES-1; j >=0; j--) {
				if (net->getCost (g_i[i], j) > 0) {
					printf ("+ x(%d,%d,%d,%d) ",j+1, g_i[i]+1,k+1, g_i[i] + 1);
				}
			}			
			printf ("\n ");
			// - sum x(r,j,k,d)
			for (int j=NODES-1; j >=0; j--) {
				if (net->getCost (g_i[i], j) > 0) {
					printf ("- x(%d,%d,%d,%d) ",g_i[i]+1,j+1,k+1, g_i[i] + 1);
				}
			}
			//= 1
			printf ("= 1\n");
		}
	}
	
}

void MultipleMulticastCommodityLP::constraint3 (rca::Network * net, 
							std::vector<std::shared_ptr<rca::Group>> & groups)
{

	//r3{k in GROUPS, d in MGROUPS[k], j in VERTEX: j <> d and j <> Mroot[k]}:
		//sum { (i,j) in LINKS} x[i, j, k, d] - 
		//sum { (j,i) in LINKS} x[j, i, k, d] = 0;

	int GROUPS = groups.size ();
	int NODES = net->getNumberNodes ();
	
	//{k in groups,
	for (int k=0; k < GROUPS; k++) {	
		
		std::vector<int> g_i = groups[k]->getMembers ();
		
		std::sort (g_i.begin (), g_i.end());
		
		int source = groups[k]->getSource ();
		
		// d in MGROUPS[K]}
		for (int i=0; i < (int)g_i.size (); i++) {
			
			
			for (int j=0; j < NODES; j++) {
		
				if (j == g_i[i]) continue;
				if (source == j) continue;
				
				printf (" r3(%d,%d,%d): ", k+1, g_i[i] +1, j+1);
			
				for (int l=NODES-1; l >= 0; l--) {
					if (net->getCost (l,j) > 0) {
						printf ("+ x(%d,%d,%d,%d) ",l+1, j+1,k+1, g_i[i] + 1);
						printf ("- x(%d,%d,%d,%d) ",j+1, l+1,k+1, g_i[i] + 1);
					}
				}
				
				//for (int l=NODES-1; l >= 0; l--) {					
				//	if (net->getCost (l,j) > 0) {
				//		printf ("- x(%d,%d,%d,%d) ",j+1, l+1,k+1, g_i[i] + 1);
				//	}
				//}
				printf ("= -0\n");
			}
			
		}
	}

}

void MultipleMulticastCommodityLP::constraint4 (rca::Network *net,
				   std::vector<std::shared_ptr<rca::Group>>&groups)
{
	
	//r4{k in GROUPS, d in MGROUPS[k], (i,j) in LINKS}:
	//	x[i,j,k,d] <= y[i,j,k];
	
	int GROUPS = groups.size ();
	int NODES = net->getNumberNodes ();
	
	//{k in groups,
	for (int k=0; k < GROUPS; k++) {	
		
		std::vector<int> g_i = groups[k]->getMembers ();
		std::sort (g_i.begin (), g_i.end());
		
		// d in MGROUPS[K]}
		for (int i=0; i < (int)g_i.size (); i++) {
			
			for (int v = 0; v < NODES; v++) {
				for (int w = 0; w < NODES; w++) {
					
					if (net->getCost (w,v) > 0) {
					
						printf (" r4(%d,%d,%d,%d):",k+1,g_i[i]+1, w+1,v+1);
						printf (" - y(%d,%d,%d)",w+1,v+1,k+1);
						printf (" + x(%d,%d,%d,%d)",w+1, v+1, k+1, g_i[i]+1);
						printf (" <= -0 \n");
						
						//printf (" r4(%d,%d,%d,%d):",k+1,g_i[i]+1, v+1,w+1);
						//printf (" - y(%d,%d,%d)",v+1,w+1,k+1);
						//printf (" + x(%d,%d,%d,%d)",v+1, w+1, k+1, g_i[i]+1);
						//printf (" <= -0 \n");
					}
					
				}
			}
		}
		
	}
	
}

void MultipleMulticastCommodityLP::out (rca::Network * net,
				   std::vector<std::shared_ptr<rca::Group>>& groups)
{
	int GROUP_SIZE = groups.size ();
	int NODES = net->getNumberNodes ();
	for(int k = 0; k < GROUP_SIZE; k++) {
	
		for (int x = 0; x < NODES; x++) {
			for (int y = 0; y < NODES; y++) {
				
				double cost = net->getCost (x,y);
				if (cost > 0.0) {
					std::cout << "out(" << (k + 1); //group k
					std::cout << "," << (x+1) << ","<< (y+1) << "):"; //edge (x,y)
					std::cout << " - y(" << (x+1) << "," << (y+1) <<","<< (k+1)<<")";
					
					for (int g : groups[k]->getMembers()) {
						
						printf (" + x(%d,%d,%d,%d)",(x+1),(y+1),(k+1),(g+1));
						
					}					
					std::cout << " >= -0\n";
				}				
			}	
		}
		
		
	}
	
}

void MultipleMulticastCommodityLP::constraint5 (rca::Network *net,
				   std::vector<std::shared_ptr<rca::Group>>&groups)
{
	
	//r5{(i,j) in LINKS}:rca::Network *net,
	//	cap[i,j] - sum{ k in GROUPS } (y[i,j,k] + y[j,i,k] )*traffic[k] >= Z;
	int GROUPS = groups.size ();
	int NODES = net->getNumberNodes ();
	
	for (int v = 0; v < NODES; v++) {
		for (int w = 0; w < v; w++) {
			
			int cap = (int)net->getBand (v,w);
			
			if (net->getBand (v,w) > 0.0) {
				
				printf (" r5(%d,%d): - Z",v+1,w+1);
				for (int k = GROUPS-1; k >=0; k--) {					
					int traffic = (int)groups[k]->getTrequest();
					printf (" - %d y(%d,%d,%d) - %d y(%d,%d,%d)", traffic, w+1,v+1,k+1, traffic, v+1, w+1,k+1);					
				}
				printf (" >= %d\n", -1*cap);	
				
				printf (" r5(%d,%d): - Z",w+1,v+1);
				for (int k = GROUPS-1; k >=0; k--) {					
					int traffic = (int)groups[k]->getTrequest();
					printf (" - %d y(%d,%d,%d) - %d y(%d,%d,%d)", traffic, v+1, w+1,k+1, traffic, w+1,v+1,k+1);					
				}
				printf (" >= %d\n", -1*cap);	
				
			}
		}
	}
	
}

void MultipleMulticastCommodityLP::bounds (rca::Network *net,
				   std::vector<std::shared_ptr<rca::Group>>&groups )
{
	
	//var y{ (i,j) in LINKS, k in GROUPS}, binary;
	//var x{ (i,j) in LINKS, k in GROUPS, d in MGROUPS[k]}, binary;

	
	int GROUPS = groups.size ();
	int NODES = net->getNumberNodes ();
	
	printf ("\nBounds\n");
	
	
	for (int k=0; k < GROUPS; k++) {
		for (int v = 0; v < NODES; v++) {
			for (int w = 0; w < v; w++) {
				
				if (net->getCost (w,v) > 0.0) {
					
					printf (" 0 <= y(%d,%d,%d) <= 1\n",v+1, w+1, k+1);
					printf (" 0 <= y(%d,%d,%d) <= 1\n",w+1, v+1, k+1);
				}
			}			
		}		
	}
	
	//{k in groups,
	for (int k=0; k < GROUPS; k++) {		
		std::vector<int> g_i = groups[k]->getMembers ();		
		// d in MGROUPS[K]}
		for (int i=0; i < (int)g_i.size (); i++) {			
			for (int v = 0; v < NODES; v++) {
				for (int w = 0; w < v; w++) {					
					if (net->getCost (w,v) > 0.0) {						
						printf (" 0 <= x(%d,%d,%d,%d) <= 1\n",v+1, w+1, k+1, g_i[i]+1);
						printf (" 0 <= x(%d,%d,%d,%d) <= 1\n",w+1, v+1, k+1, g_i[i]+1);						
					}					
				}				
			}			
		}		
	}
	
	printf ("\n\nGENERALS\n");
	
	for (int k=0; k < GROUPS; k++) {
		for (int v = 0; v < NODES; v++) {
			for (int w = 0; w < v; w++) {
				if (net->getCost (w,v) >0) {
					printf (" y(%d,%d,%d)\n",v+1, w+1, k+1);
					printf (" y(%d,%d,%d)\n",w+1, v+1, k+1);
				}
			}			
		}		
	}
	
	//{k in groups,
	for (int k=0; k < GROUPS; k++) {		
		std::vector<int> g_i = groups[k]->getMembers ();		
		// d in MGROUPS[K]}
		for (int i=0; i < (int)g_i.size (); i++) {
			for (int v = 0; v < NODES; v++) {
				for (int w = 0; w < v; w++) {
					if (net->getCost (w,v) >0) {
						printf (" x(%d,%d,%d,%d)\n",v+1, w+1, k+1, g_i[i]+1);
						printf (" x(%d,%d,%d,%d)\n",w+1, v+1, k+1, g_i[i]+1);							
					}					
				}				
			}			
		}		
	}
	
	//printf ("\n\nGeneral\n");
	printf (" Z\n\nEnd");
	
}

void MultipleMulticastCommodityLP::constraint7 (rca::Network *net,
				   std::vector<std::shared_ptr<rca::Group>>&groups )
{
	
	//VOID CIRCLE
	//r7{i in VERTEX, k in GROUPS}:
	//	sum { (j,m) in LINKS: m=i and m <> Mroot[k]} y[j,m,k] <=1;

	int GROUPS = groups.size ();
	int NODES = net->getNumberNodes ();
	
	for (int i=0; i < NODES; i++) {
	
		for (int k =0; k < GROUPS; k++) {
		
			if ( i == groups[k]->getSource ()) {
				continue;
			}
			
			printf (" r7(%d,%d):",i+1,k+1);
			
			for (int j=0; j < NODES; j++) {
				
				if (net->getCost (i,j) > 0.0) {
					printf (" + y(%d,%d,%d)",j+1,i+1,k+1);
				}
			}
			
			printf (" <= 1\n");
		
		}
		
	}

}

void MultipleMulticastCommodityLP::constraint8 (rca::Network *net,
				   std::vector<std::shared_ptr<rca::Group>>&groups)
{

	//#fix incoming flow on the root of k
	//r8{ (i,j) in LINKS, k in GROUPS: Mroot[k] = j}:
	//	y[i,j,k] = 0;
	
	int GROUPS = groups.size ();
	int NODES = net->getNumberNodes ();
	
	//(i,j) in LINKS
	for (int i = 0; i < NODES; i++) {
		for (int j = 0; j < NODES ; j++) {
			
			//K IN groups
			for (int k = 0; k < GROUPS; k++) {
				
				if ( net->getCost(i,j) > 0 && j == groups[k]->getSource()) {
					printf (" r8(%d,%d,%d): + y(%d,%d,%d) = -0\n",i+1,j+1,k+1,i+1,j+1,k+1);
				}
			}
		}
		
	}
	
}

void MMSTPBudgetLP::constraint6 (rca::Network *net,
				   std::vector<std::shared_ptr<rca::Group>>&groups)
{
	
	int NODES = net->getNumberNodes ();
	int GROUPS = groups.size ();

	std::cout << " r6:";
	for (int k = 0; k < GROUPS; k++) {
		for ( int i=0; i < NODES; i++) {	
			//for ( int j=0; j < NODES; j++) { com ESTE FOR É POSSÍVEL TER
			//O BUDGET POR ÁRVER.
			//A IMPLMENTAÇÃO ATUAL CALCULA POR SOLUÇÃO
			for ( int j=0; j < i; j++) { 
				int cost = net->getCost (i,j);
				if ( cost > 0 ) {
					printf (" + %d y(%d,%d,%d)",cost,i+1,j+1,k+1);
					printf (" + %d y(%d,%d,%d)",cost,j+1,i+1,k+1);
				}
			}
		}
	}
	printf (" <= %d\n", m_budget);
	
}

void MultipleMulticastCommodityLP::delay (rca::Network * net, 
					std::vector<std::shared_ptr<rca::Group>>& groups)
{

	int GROUPS = groups.size ();
	int NODES = net->getNumberNodes ();
	int delay = 5;
	
	//{k in groups,
	for (int k=0; k < GROUPS; k++) {	
		
		std::vector<int> g_i = groups[k]->getMembers ();		
		std::sort (g_i.begin (), g_i.end());
		//int source = groups[k]->getSource ();
		// d in MGROUPS[K]}
		for (int i=0; i < (int)g_i.size (); i++) {
			
			printf (" delay(%d,%d): ", k+1, g_i[i] +1);
			
			for (int v = 0; v < NODES; v++) {
				for (int w = 0; w < v; w++) {					
					if (net->getCost (w,v) > 0.0) {						
						printf (" + x(%d,%d,%d,%d) ",v+1, w+1, k+1, g_i[i]+1);
						printf (" + x(%d,%d,%d,%d) ",w+1, v+1, k+1, g_i[i]+1);						
					}					
				}				
			}	

			printf (" <= %d\n", delay);
		}

	}

}

void MMSTPBudgetLP::generate(rca::Network *network,
							 std::vector<std::shared_ptr<rca::Group>>&groups)
{
	std::cout << "Maximize\n objective: + Z\n\nSubject To\n";
	
	constraint1 (network, groups);
	constraint2 (network, groups);
	constraint3 (network, groups);
	
	constraint4 (network, groups);
	out (network, groups);
	
	constraint5 (network, groups);
	constraint7 (network, groups);
	
	//this restriction is not necessary
	//evitar que chegue fluxo na fonte
	//constraint8 (network, groups);
	
	//budget_constraint
 	constraint6 (network, groups);
	
 	//delay (network, groups);

	bounds (network, groups);

	
}

//-------------------------------- MODELO OTIMIZADO POR CUSTO ----------------------//

void MMSTPCostLP::constraint6 (rca::Network *net,
				   std::vector<std::shared_ptr<rca::Group>>&groups) { 
	

	int GROUPS = groups.size ();
	int NODES = net->getNumberNodes ();
	
	for (int v = 0; v < NODES; v++) {
		for (int w = 0; w < v; w++) {
			
			int cap = (int)net->getBand (v,w);
			
			if (net->getBand (v,w) > 0.0) {
				
				printf (" r5(%d,%d): ",v+1,w+1);
				for (int k = GROUPS-1; k >=0; k--) {					
					int traffic = (int)groups[k]->getTrequest();
					printf (" - %d y(%d,%d,%d) - %d y(%d,%d,%d)", traffic, w+1,v+1,k+1, traffic, v+1, w+1,k+1);					
				}
				printf (" >= %d\n", Z - cap);	
				
				printf (" r5(%d,%d): ",w+1,v+1);
				for (int k = GROUPS-1; k >=0; k--) {					
					int traffic = (int)groups[k]->getTrequest();
					printf (" - %d y(%d,%d,%d) - %d y(%d,%d,%d)", traffic, v+1, w+1,k+1, traffic, w+1,v+1,k+1);					
				}
				printf (" >= %d\n", Z - cap);	
				
			}
		}
	}
	
}

void MMSTPCostLP::generate(rca::Network *network,
							 std::vector<std::shared_ptr<rca::Group>>&groups)
{
// 	std::cout << "Maximize\n objective: + Z\n\nSubject To\n";
	
	std::cout << "Minimize\n objective: ";
	int NODES = network->getNumberNodes ();
	int GROUPS = groups.size ();

	for (int k = 0; k < GROUPS; k++) {
		for ( int i=0; i < NODES; i++) {	
			//for ( int j=0; j < NODES; j++) { com ESTE FOR É POSSÍVEL TER
			//O BUDGET POR ÁRVER.
			//A IMPLMENTAÇÃO ATUAL CALCULA POR SOLUÇÃO
			for ( int j=0; j < i; j++) { 
				int cost = network->getCost (i,j);
				if ( cost > 0 ) {
					printf (" + %d y(%d,%d,%d)",cost,i+1,j+1,k+1);
					printf (" + %d y(%d,%d,%d)",cost,j+1,i+1,k+1);
				}
			}
		}
	}
	std::cout << "\nSubject To\n";
	
	
	constraint1 (network, groups);
	
	constraint2 (network, groups);
	
	constraint3 (network, groups);
	
	constraint4 (network, groups);
	out (network, groups);
	
	//constraint5 (network, groups);
	
	constraint7 (network, groups);
	
	//this restriction is not necessary
	//evitar que chegue fluxo na fonte
	//constraint8 (network, groups);
	
	constraint6 (network, groups);
	
	//delay (network, groups);

	bounds (network, groups);


	
	
}

double LeeAndChooModel::m_alpha;
std::vector<int> LeeAndChooModel::m_opt;
void LeeAndChooModel::generate (rca::Network *network,
				   std::vector<std::shared_ptr<rca::Group>>&groups) 
{
	std::cout << "Maximize\n objective: + Z\n\nSubject To\n";
	
	constraint1 (network, groups);
	constraint2 (network, groups);
	constraint3 (network, groups);
	
	constraint4 (network, groups);
	out (network, groups);
	
	constraint5 (network, groups);
	constraint7 (network, groups);
	
	//constraint of Choo and Lee 2004
	
	int NODES = network->getNumberNodes ();
	for (size_t k=0; k < groups.size (); k++) {
	
			std::cout << " Lee("<<k+1<<"):";
		
			for (int i=0; i < NODES; i++) {
				for (int j=0; j < NODES; j++) {
					
					int cost = network->getCost (i,j);
					
					if (cost > 0) {
						std::cout << " + " << cost << " y(";
						std::cout << i+1 << "," << j+1 << "," << k+1 << ")";
					}
					
				}
			}
			
			std::cout << " <= " << (int)(m_alpha * m_opt[k]) << std::endl;
		
	}
	
	bounds (network, groups);
}