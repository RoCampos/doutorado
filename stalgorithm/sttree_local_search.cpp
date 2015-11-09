
#include "sttree_local_search.h"

using namespace rca::sttalgo;

template<class NetworkType>
void rca::sttalgo::depth_first_search<NetworkType>::execute (
								std::vector<rca::Link>& links, 
								rca::Group& g, 
								rca::Link& link, 
								rca::Network & network)
{
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	
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
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	
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
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
	
	int NODES = m_network.getNumberNodes ();
	int GSIZE = m_groups.size ();
	
	int tcost = m_trees[tree].getCost ();
	int old = tcost;
	
	std::set<int> vertex;	
	std::vector<rca::Link> m_links;
	
	// Criando uma lista de arestas e removendo-as do container.
	// Estas arestas correspondem a árvore multicast tree.

	edge_t * e = m_trees[tree].get_edge ();
	while (e != NULL) {
		if (e->in) {
			
			rca::Link link (e->x, e->y, 0);
			
			// Se a aresta é utilizada apenas uma vez
			//remove-a do grafo
			if (cg.value (link) == (GSIZE-1) ) {
				cg.erase (link);
			} else {

				// Se ele é utilizada por mais de uma árvore
				// Então, desconta um valor de unidade
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
	

	//inicia busca por links que formem ciclos
	for (auto v : vertex) {
		for (auto w: vertex) {

			if (v < w) {
				
				//verificar se o par de vértices forma um lik
				if (m_network.getCost (v,w) > 0) {
					
					//cria o link para incluir um ciclos
					rca::Link link (v,w, (int) m_network.getCost (v,w));
					

					//verifica se a utilização do link 
					//vai não vai piorar a qualidade da solução
					if (cg.is_used(link)) {
						
						if (cg.value (link) <= cg.top ()) continue;
						
					}
					
					//verifica se o link estava na árvore
					//onde ele será inserido
					if (std::find(m_links.begin(), m_links.end(), link) 
						== m_links.end()) {
#ifdef DEBUG11						
						std::string s('-', 60);
						std::cout << s << std::endl;
						
						std::cout << "Circle Induced by : " << link << std::endl;
#endif

						
						// Obtém as arestas que formam o ciclo
						std::vector<rca::Link> toRemove;
						toRemove = get_circle (m_links, 
										m_groups[tree], 
										link, m_network);
						
						//adding the links an remove others
						// Os links presentes em 'toRemove' são 
						// os links que possuem x,y coomo não terminais
						// Assim força a remoção de mais links considerando
						// que os links que ligados x,y serão também removidos

						rca::Group g = m_groups[tree];
						for (auto l : toRemove) {
#ifdef DEBUG11				
							std::cout <<"Removing "<<l << std::endl;
#endif				
							// Adiciona a árvore apenas os links da árvore
							// sem o link que 'x,y' 
							// devidos suas caracterísicas, ou seja 
							// não possui nehum vértices terminal
							// então vai permitir que mais links sejam 
							// removidos
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
#ifdef DEBUG11
							steiner_tree.xdotFormat ();
#endif
							
#ifdef DEBUG11
							std::cout << tcost << " ";
							std::cout << steiner_tree.getCost () << std::endl;
#endif			
							if (tcost > steiner_tree.getCost ()) {
								m_trees[tree] = steiner_tree;
								tcost = steiner_tree.getCost ();
							}
#ifdef DEBUG11							
 							getchar ();
#endif
						}
						
#ifdef DEBUG11
						std::cout << s << std::endl;					
						std::cout << std::endl;
#endif
					}
					
				}
			}
			
		}
	}
	
#ifdef DEBUG11
	std::cout << old <<" "<< m_trees[tree].getCost () << std::endl;
#endif
	
	//atualiza container com a nova árvore
	// é necesário, pois nenhum observer está sendo
	// utilizado aqui
	// apenas a estrutura que representa a árvore de steiner
	
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
#ifdef DEBUG1
	std::cout << __FUNCTION__ << std::endl;
#endif
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
	
	return toRemove;
	
}
template <class Container>
void cycle_local_search<Container>::local_search (std::vector<STTree> & m_trees, 
											rca::Network& m_network, 
											std::vector<rca::Group>& m_groups, 
											Container& cg, 
											int & cost)
{
#ifdef DEBUG
	std::cout << __FUNCTION__ << std::endl;
#endif
	
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

#ifdef DEBUG
	std::cout << "endof " << __FUNCTION__ << std::endl;
#endif
	
}

//***********************************************************************/
template class cycle_local_search<rca::EdgeContainer<rca::Comparator, rca::HCell>>;

template class depth_first_search<rca::AdjacentNetwork>;
