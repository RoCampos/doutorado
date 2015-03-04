#include "genetic_algorithm.h"
#include "metaheuristic.h"

void GeneticAlgorithm::run_metaheuristic (std::string instance)
{
	/*configuring the problem*/
	m_instance = instance;
	m_network = new Network;
	Reader r (m_instance);
	r.configNetwork ( m_network );
	std::vector<shared_ptr<rca::Group>> g = r.readerGroup ();
	for (unsigned int i =0; i < g.size (); i++) {
		m_groups.push_back (*g[i].get ());
	}
	
	/*configuring parameters - default*/
	init_parameters ();
	
	/*init population*/
	init_population ();
	
	
	
	//deallocatin of resources;
	delete m_network;
}

void GeneticAlgorithm::init_population ()
{
#ifdef DEBUG
	std::cout << "Creating Population: size = " << m_pop << std::endl;	
#endif
	
	int best = -1;
	int max = 0;
	int cost = 10000000;
	m_population = std::vector<PathRepresentation> (m_pop);
	for (int i=0; i < m_pop; i++) {
		m_population[i].init_rand_solution (m_network, m_groups);
		
		if (m_population[i].getCost () < 18545) {
		
			if (max < m_population[i].getResidualCap ()) {
			
				best = i;
				max =  m_population[i].getResidualCap ();
			} else if (max == m_population[i].getResidualCap ()) {
			
				if (m_population[i].getCost () < cost) {
					cost = m_population[i].getCost ();
					best = i;
				}
			}
			
		}
		
	}
	
	std::cout << m_population[best].getCost () << std::endl;
	std::cout << m_population[best].getResidualCap () << std::endl;
	
}

void PathRepresentation::init_rand_solution (rca::Network * net, 
									  std::vector<rca::Group>& groups)
{
	
	/*the overall cost of solution*/
	m_cost = 0;
	
	std::vector<rca::Link> used_links;
	
	int GROUPS = groups.size ();
	int NODES = net->getNumberNodes ();
	for (int i=0; i < GROUPS; i++) {
		int source = groups[i].getSource ();	
		/*Steiner tree to compute correctly cost*/
		SteinerTree st (net->getNumberNodes (), source, groups[i].getMembers());
		
		/*disjoint set to avoid circle*/
		//TODO IMPLEMENTAR DENTRO DA ST_IMPLEMENTAÇÃO
		DisjointSet2 dset (NODES);
	
		rca::Path path;
		int w = groups[i].getMember (0);
		path = shortest_path (source, w, net);
#ifdef DEBUG
		std::cout << "TREE :" << i << std::endl;
		printf ("\tpath %d to %d ", source, w);
		std::cout << path << std::endl;
#endif		
		for (int d=0; d < groups[i].getSize (); ) {
			
			d++;
			
			std::vector<rca::Link> links;
			int rnd = rand () % 10 + 1;
			if (rnd < 5) {

				/* removing this path */
				std::vector<int>::reverse_iterator it = path.rbegin ();
				for (; it!= path.rend () -1; it++) {
					rca::Link link( (*it), *(it + 1), 0);
					net->removeEdge (link);
					
					links.push_back (link);
				}
#ifdef DEBUG
				std::cout << "\t\tremoving path ";
				std::cout << path << " size (";
				std::cout << path.size () << ")\n";
#endif
			}
			
			m_genotype.push_back (path);
			
			std::vector<int>::reverse_iterator it = path.rbegin ();
			for (; it!= path.rend () -1; it++) {
				//rca::Link link( (*it), *(it + 1), 0);
				int v = *it;
				int x = *(it+1);
				
				if ( dset.find2 (v) != dset.find2 (x) ) {
					
					dset.simpleUnion (v, x);
					
					(v > x ? st.addEdge (v, x, (int)net->getCost (v,x)):
							st.addEdge (x, v, (int)net->getCost (v,x)));
				
				}
			}
		
			if (d == groups[i].getSize ()) break;
			
			w = groups[i].getMember (d);
			path = shortest_path (source, w, net);
#ifdef DEBUG
		printf ("\tpath %d to %d ", source, w);
		std::cout << path << std::endl;
#endif
			
			std::vector<rca::Link>::iterator itl = links.begin ();
			for (; itl!= links.end (); itl++) {
				net->undoRemoveEdge (*itl);
			}		
		}
		st.prunning ();
		m_cost += st.getCost ();
		
#ifdef DEBUG
		st.xdotFormat ();
		getchar ();
#endif
		
		/*computing the usage*/
		Edge * e = st.listEdge.head;
		while ( e != NULL) {
			
			int trequest = groups[i].getTrequest ();
			rca::Link link (e->i, e->j, 0);
			
			std::vector<rca::Link>::iterator it;
			it = std::find (used_links.begin (), used_links.end(),link);
			if (it != used_links.end()) {
				it->setValue ( it->getValue () - trequest);
			} else {
				int band = net->getBand (e->i, e->j);
				link.setValue (band);
				used_links.push_back (link);
			}
			
			e = e->next;
		}
	
		net->clearRemovedEdges ();
	}
	
	
	/*udpating residual capacity*/
	//int max = std::numeric_limits<int>::max();
	//std::sort (used_links.begin (), used_links.end());
	
	rca::Link link =  *(std::min(used_links.begin(), used_links.end()));
	m_residual_capacity = link.getValue ();
	
}

int main (int argc, char**argv) 
{
	srand (1);
	
	std::string instance = argv[1];
	MetaHeuristic<GeneticAlgorithm> algorithm;
	
	
	algorithm.run (instance);
	
}