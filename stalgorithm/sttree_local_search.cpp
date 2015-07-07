
#include "sttree_local_search.h"

using namespace rca::sttalgo;

template<class NetworkType>
void rca::sttalgo::depth_first_search<NetworkType>::execute (
								std::vector<rca::Link>& links, 
								rca::Group& g, 
								rca::Link& link, 
								rca::Network & network)
{
#define WHITE 0
#define GREY 1
#define BLACK 2
	
	this->x = link.getX();
	this->y = link.getY();
	
	this->stop = false;
	
	int NODES = network.getNumberNodes ();
	
	NetworkType m_network(NODES, links);
	
	predecessor = std::vector<int> (NODES, -1);
	color = std::vector<int>(NODES, WHITE);
	
	predecessor[x] = x;
	color[x] = GREY;
	dfs (x, g, m_network);
	color[x] = BLACK;
	
}

template<class NetworkType>
void rca::sttalgo::depth_first_search<NetworkType>::dfs (
								int x, 
								rca::Group& g, 
								NetworkType & network)
{

	
	if (x == this->y) {
		this->stop = true; //break condition
	}
	
 	std::vector<int> neighboor = network.outEdge (x);
	
	for (int i = 0; i < neighboor.size () && this->stop == false; i++) {			

		if ( color [ neighboor[i] ] == WHITE ) {
			predecessor[ neighboor[i] ] = x;
			
			color[ neighboor[i] ] = GREY;
			this->dfs ( neighboor[i], g, network ); //apply dfs in neighboor of x
			color[ neighboor[i] ] = BLACK;
		}
	}
	
	
}

/* -------------------- CYCLE LOCAL SEARCH ALGORITHMS ---------------------*/
template<typename Container>
void cycle_local_search<Container>::execute ( int tree, 
											std::vector<STTree> & m_trees, 
											rca::Network& m_network, 
											std::vector<rca::Group>& m_groups, 
											Container& cg)
{
	
	int NODES = m_network.getNumberNodes ();
	int GSIZE = m_groups.size ();
	
	int tcost = m_trees[tree].getCost ();
	int old = tcost;
	
	std::set<int> vertex;	
	std::vector<rca::Link> m_links;
	
	edge_t * e = m_trees[tree].get_edge ();
	while (e != NULL) {
		if (e->in) {
			
			rca::Link link (e->x, e->y, 0);
			
			if (cg.value (link) == (GSIZE-1) ) {
				cg.erase (link);
			} else {
				int value = cg.value (link) + 1;
				cg.erase (link);
				link.setValue (value);
				cg.push (link);
			}
			
			vertex.insert (link.getX());
			vertex.insert (link.getY());
			
			m_links.push_back (link);
			
		}
		e = e->next;
	}
	
	for (auto v : vertex) {
		for (auto w: vertex) {
			
			if (v < w) {
				
				if (m_network.getCost (v,w) > 0) {
					
					
					rca::Link link (v,w, (int) m_network.getCost (v,w));
					
					if (cg.is_used(link)) {
						
						if (cg.value (link) <= cg.top ()) continue;
						
					}
					
					
					if (std::find(m_links.begin(), m_links.end(), link) 
						== m_links.end()) {
#ifdef DEBUG						
						std::string s('-', 60);
						std::cout << s << std::endl;
						
						std::cout << "Circle Induced by : " << link << std::endl;
#endif
						std::vector<rca::Link> toRemove;
						toRemove = get_circle (m_links, 
										m_groups[tree], 
										link, m_network);
						
						//adding the links an remove others
						rca::Group g = m_groups[tree];
						for (auto l : toRemove) {
#ifdef DEBUG				
							std::cout <<"Removing "<<l << std::endl;
#endif				
							int source = g.getSource ();
							STTree steiner_tree(NODES, source, g.getMembers ());
							for (auto ll : m_links) {
							
								if (ll != l) {
									int cost = m_network.getCost (ll.getX(), 
																  ll.getY());
									steiner_tree.add_edge (ll.getX(), 
														   ll.getY(),
														   cost);
								}
								
							}
							int cost = m_network.getCost(link.getX(), link.getY());
							steiner_tree.add_edge (link.getX(), link.getY(), cost);
							
							steiner_tree.prunning ();
#ifdef DEBUG
							steiner_tree.xdotFormat ();
#endif
							
#ifdef DEBUG
							std::cout << tcost << " ";
							std::cout << steiner_tree.getCost () << std::endl;
#endif			
							if (tcost > steiner_tree.getCost ()) {
								m_trees[tree] = steiner_tree;
								tcost = steiner_tree.getCost ();
							}
#ifdef DEBUG							
 							getchar ();
#endif
						}
						
#ifdef DEBUG
						std::cout << s << std::endl;					
						std::cout << std::endl;
#endif
					}
					
				}
			}
			
		}
	}
	
#ifdef DEBUG
	std::cout << old <<" "<< m_trees[tree].getCost () << std::endl;
#endif
	
	e = m_trees[tree].get_edge ();
	while ( e != NULL) {
			
		if (e->in) {
			
			rca::Link link (e->x, e->y, 0);
			
			if (cg.is_used (link)) {
				
				int value = cg.value (link) - 1;
				
				cg.erase (link);
				link.setValue (value);
				cg.push (link);
				
			} else {
				link.setValue (GSIZE - 1); 
				cg.push (link);
			}
		}
		
		e = e->next;
	}
	
	
}

template <class Container>
std::vector<rca::Link> 
cycle_local_search<Container>::get_circle (std::vector<rca::Link>& links, 
				rca::Group& group, 
				rca::Link& link, 
				rca::Network & m_network)
{

// #define WHITE 0
// #define GREY 1
// #define BLACK 2
	
// 	int x = link.getX();
// 	int y = link.getY();
	
// 	int NODES = m_network.getNumberNodes ();
	
// 	std::vector<int> predecessor(NODES, -1);
// 	std::vector<int> visited(NODES, 0);
// 	std::vector<int> color(NODES, WHITE);
// 	std::vector<int> time(NODES, 0);
	
// 	std::vector<int> stack;
// 	
// 	stack.push_back (x);
	
// 	int tempo = 0;
	
// 	std::vector<rca::Link> m_links = links;
	
	rca::sttalgo::depth_first_search<AdjacentNetwork> dfs;
	dfs.execute (links, group, link, m_network);
	
	int xx = link.getX();
	int yy = link.getY();
	
	std::vector<rca::Link> toRemove;
	
	std::vector<int> prev = dfs.getPredecessorList ();
	
	int source = group.getSource ();
	while ( yy != dfs.getStart()) {
		
		rca::Link l(yy,prev[yy],0);
		if ( !group.isMember(l.getX()) && 
			!group.isMember(l.getY()) 
			&& source != l.getY() && source != l.getX()) {
			
			toRemove.push_back (l);
 			break;		
		}
		
		yy = prev[yy];
	}
	
	//busca em profundidade iniciando em x até alcançar x novamente.
	//usada para detectar um ciclo.
// 	while ( !stack.empty() ) {
// 		
// 		int current_node = stack[ stack.size() -1 ];
// 		
// 		if (current_node == y) break;
// 		
// 		tempo++;
// 		time[current_node] = tempo;
// 		color[current_node] = GREY;
// 		
// 		bool found = false;
// 		int l = 0;
// 		
// 		
// 		for (auto link : m_links) {
// 			
// 			int xx = link.getX();
// 			int yy = link.getY();
// 			if (xx == current_node && color[yy] == WHITE) {
// 			
// 				predecessor [yy] = current_node;
// 				stack.push_back (yy);
// 				found = true;
// 				
// 				//erase a link after visited the vertex yy by xx
// 				m_links.erase (m_links.begin()+l); 
// 				break;
// 			} 
// 			
// 			if (yy == current_node && color[xx] == WHITE) {
// 			
// 				predecessor [xx] = current_node;
// 				stack.push_back (xx);
// 				found = true;
// 				
// 				//erase a link after visited the vertex xx by yy
// 				m_links.erase (m_links.begin()+l); 
// 				break;
// 			} 
// 			
// 			l++; //link counter
// 		}
// 		
// 		if (found) {
// 			continue;
// 		}
// 		
// 		color[current_node] = BLACK;
// 		tempo++;
// 		time[current_node] = tempo;
// 		
// 		stack.erase ( stack.begin() + (stack.size () - 1) );
// 		
// 	}
// 	
// 	std::vector<rca::Link> toRemove;
// 	
// 	for (int i : stack) {
// 		std::cout << i << " ";
// 	}
// 	std::cout << std::endl;
// 	getchar ();
// 	
// 	int source = group.getSource ();
// 	for (int i=stack.size () - 1; i >=1; i--) {
// 		
// 		int x = stack[i];
// 		int y = stack[i-1];
// 		
// 		rca::Link l(x,y,0);
// 		
// 		if ( !group.isMember(x) && !group.isMember(y) && source != x && source != y) {
// 			toRemove.push_back (l);
//  			break;
// 		}
// 	}
	
	return toRemove;
	
}
template <class Container>
void cycle_local_search<Container>::local_search (std::vector<STTree> & m_trees, 
											rca::Network& m_network, 
											std::vector<rca::Group>& m_groups, 
											Container& cg, 
											int & cost)
{

	std::vector<int> idx(m_groups.size(), 0);
	std::iota(idx.begin(), idx.end(), 0);
	
	int improve = cost;
	do {
		improve = cost;
		cost = 0;
		std::random_shuffle (idx.begin(), idx.end());
		for (int i: idx) {
			this->execute (i, m_trees, m_network, 
 			   m_groups, cg);		
			cost += m_trees[i].getCost ();
		}

	}while (cost < improve);
	
}

//***********************************************************************/
template class cycle_local_search<rca::EdgeContainer<rca::Comparator, rca::HCell>>;

template class depth_first_search<rca::AdjacentNetwork>;
