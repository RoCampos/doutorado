#include "SolverDataGenerator.h"

void MultiCommidityFormulation::generate (rca::Network * network, 
										  rca::Group *g)
{
	
	std::cout << "set V := ";
	for (int i=0;i < network->getNumberNodes (); i++) {
		std::cout << i + 1 << " ";
	}
	std::cout << ";\n";

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
	
	std::cout << "\nset T := ";
	for (int i=0; i < g->getSize (); i++) {
		std::cout << " " << g->getMember (i) + 1;
	}
	std::cout << " ;\n";
	
	std::cout << "param r := " << g->getSource ()+1 << ";\n";
	
	std::cout << "param : cost delay:=";
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
	std::cout << "Maximize\nobjective: + Z\n\nSubject to\n";
	
	//constraint1 (network, groups);
	//constraint2 (network, groups);
	//constraint3 (network, groups);
	//constraint4 (network, groups);
	//constraint5 (network, groups);
	//void constraint6 ();
	constraint7 (network, groups);
	//void constraint8 ();
	
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
		int source = groups[k]->getSource ();
		// d in MGROUPS[K]}
		for (int i=0; i < (int)g_i.size (); i++) {
			
			printf (" r1(%d,%d): ", k+1, g_i[i] +1);
			
			//SUM x(j,r,k,d)
			for (int j=0; j < NODES; j++) {
				if (net->getCost (source, j) > 0) {
					printf ("+ x(%d,%d,%d,%d) ",j+1, source+1,k+1, g_i[i] + 1);
				}
			}			
			printf ("\n ");
			// - sum x(r,j,k,d)
			for (int j=0; j < NODES; j++) {
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
		//int source = groups[k]->getSource ();
		// d in MGROUPS[K]}
		for (int i=0; i < (int)g_i.size (); i++) {
			
			printf (" r2(%d,%d): ", k+1, g_i[i] +1);
			
			//SUM x(j,r,k,d)
			for (int j=0; j < NODES; j++) {
				if (net->getCost (g_i[i], j) > 0) {
					printf ("+ x(%d,%d,%d,%d) ",j+1, g_i[i]+1,k+1, g_i[i] + 1);
				}
			}			
			printf ("\n ");
			// - sum x(r,j,k,d)
			for (int j=0; j < NODES; j++) {
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
		int source = groups[k]->getSource ();
		
		// d in MGROUPS[K]}
		for (int i=0; i < (int)g_i.size (); i++) {
			
			
			for (int j=0; j < NODES; j++) {
		
				if (groups[k]->isMember( j )) continue;
				if (source == j) continue;
				
				printf (" r3(%d,%d,%d): ", k+1, g_i[i] +1, j+1);
			
				for (int l=0; l < NODES; l++) {
					if (net->getCost (l,j) > 0) {
						printf ("+ x(%d,%d,%d,%d) ",l+1, j+1,k+1, g_i[i] + 1);
					}
				}
				
				for (int l=0; l < NODES; l++) {
					
					if (net->getCost (l,j) > 0) {
						printf ("- x(%d,%d,%d,%d) ",j+1, l+1,k+1, g_i[i] + 1);
					}
				}
				printf ("= 0\n");
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
		
		// d in MGROUPS[K]}
		for (int i=0; i < (int)g_i.size (); i++) {
			
			for (int v = 0; v < NODES; v++) {
				for (int w = 0; w < NODES; w++) {
					
					if (net->getCost (w,v) >0) {
					
						printf (" r4(%d,%d,%d,%d):",k+1,g_i[i]+1, w+1,v+1);
						printf (" - y(%d,%d,%d)",w+1,v+1,k+1);
						printf (" + x(%d,%d,%d,%d)",w+1, v+1, k+1, g_i[i]+1);
						printf (" <= -0 \n");
					}
					
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
				for (int k = 0; k < GROUPS; k++) {					
					int traffic = (int)groups[k]->getTrequest();
					printf (" - %d y(%d,%d,%d) - %d y(%d,%d,%d)", traffic, v+1,w+1,k+1, traffic, w+1, v+1,k+1);					
				}
				printf (" >= %d\n", -1*cap);	
				
				printf (" r5(%d,%d): - Z",w+1,v+1);
				for (int k = 0; k < GROUPS; k++) {					
					int traffic = (int)groups[k]->getTrequest();
					printf (" - %d y(%d,%d,%d) - %d y(%d,%d,%d)", traffic, w+1, v+1,k+1, traffic, v+1,w+1,k+1);					
				}
				printf (" >= %d\n", -1*cap);	
				
			}
		}
	}
	
}

void MultipleMulticastCommodityLP::bounds (rca::Network *net,
				   std::vector<std::shared_ptr<rca::Group>>&groups )
{
	
	int GROUPS = groups.size ();
	int NODES = net->getNumberNodes ();
	
	printf ("\nBounds\n");
	
	
	for (int k=0; k < GROUPS; k++) {
		for (int v = 0; v < NODES; v++) {
			for (int w = 0; w < NODES; w++) {
				if (net->getCost (w,v) >0) {
					printf ("0 <= y(%d,%d,%d) <= 1\n",v+1, w+1, k+1);
					printf ("0 <= y(%d,%d,%d) <= 1\n",w+1, v+1, k+1);
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
				for (int w = 0; w < NODES; w++) {					
					if (net->getCost (w,v) >0) {						
						printf ("0 <= x(%d,%d,%d,%d) <= 1\n",v+1, w+1, k+1, g_i[i]+1);
						printf ("0 <= x(%d,%d,%d,%d) <= 1\n",w+1, v+1, k+1, g_i[i]+1);						
					}					
				}				
			}			
		}		
	}
	
	printf ("\n\nGenerals\n");
	
	for (int k=0; k < GROUPS; k++) {
		for (int v = 0; v < NODES; v++) {
			for (int w = 0; w < NODES; w++) {
				if (net->getCost (w,v) >0) {
					printf ("y(%d,%d,%d)\n",v+1, w+1, k+1);
					printf ("y(%d,%d,%d)\n",w+1, v+1, k+1);
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
				for (int w = 0; w < NODES; w++) {					
					if (net->getCost (w,v) >0) {						
						printf ("x(%d,%d,%d,%d)\n",v+1, w+1, k+1, g_i[i]+1);
						printf ("x(%d,%d,%d,%d)\n",w+1, v+1, k+1, g_i[i]+1);							
					}					
				}				
			}			
		}		
	}
	
	
	printf ("Z\n\nEND");
	
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
			
			printf (" r7(%d,%d): ",i+1,k+1);
			
			for (int j=0; j < NODES; j++) {
				
				if (net->getCost (i,j) > 0.0) {
					printf (" + (%d,%d,%d)",j+1,i+1,k+1);
				}
			}
			
			printf (" <= 1\n");
		
		}
		
	}
	
	
}