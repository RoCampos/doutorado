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
	
	m_population = std::vector<PathRepresentation> (m_pop);
	for (int i=0; i < m_pop; i++) {
		m_population[i].init_rand_solution (m_network, m_groups);
	}
	
}

void PathRepresentation::init_rand_solution (rca::Network * net, 
									  std::vector<rca::Group>& groups)
{
#ifdef DEBUG1
	std::cout << "genotype size = ";
	int count = 0;
#endif
	
	/*the overall cost of solution*/
	m_cost = 0;
	
	int GROUPS = groups.size ();
	for (int i=0; i < GROUPS; i++) {
		int source = groups[i].getSource ();
#ifdef DEBUG1
	count += groups[i].getSize ();
#endif
		
		SteinerTree st (net->getNumberNodes (), source, groups[i].getMembers());
	
		rca::Path path;
		int w = groups[i].getMember (0);
		path = shortest_path (source, w, net);
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
				std::cout << "removing path" << std::endl;
				std::cout << path << " size (";
				std::cout << path.size () << ")\n";
#endif
			}
			
			m_genotype.push_back (path);
			std::cout << path << std::endl;
			std::vector<int>::reverse_iterator it = path.rbegin ();
			for (; it!= path.rend () -1; it++) {
				//rca::Link link( (*it), *(it + 1), 0);
				int v = *it;
				int w = *(it+1);
				(v > w ? st.addEdge (v, w, (int)net->getCost (v,w)):
						st.addEdge (w, v, (int)net->getCost (v,w)));
			}
		
			if (d == groups[i].getSize ()) break;
			
			w = groups[i].getMember (d);
			path = shortest_path (source, w, net);
			
			std::vector<rca::Link>::iterator itl = links.begin ();
			for (; itl!= links.end (); itl++) {
				net->undoRemoveEdge (*itl);
			}		
		}
		st.prunning ();
		m_cost += st.getCost ();
		st.xdotFormat ();
		
		getchar ();
		
	}
	
#ifdef DEBUG1
	std::cout << (count) << std::endl;
#endif
	
}

int main (int argc, char**argv) 
{
	srand (1);
	
	std::string instance = argv[1];
	MetaHeuristic<GeneticAlgorithm> algorithm;
	
	
	algorithm.run (instance);
	
}