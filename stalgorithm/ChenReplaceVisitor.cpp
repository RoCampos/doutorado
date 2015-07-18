#include "mpp_visitor.h"

using namespace rca;
using namespace rca::sttalgo;

void ChenReplaceVisitor::visit ()
{
#ifdef DEBUG1
	std::cout <<"----------"<<__FUNCTION__ <<"-------------"<< std::endl;
#endif
	
	//passar árvores de steiner de SteinerTree para
	//lista de arestas
	prepare_trees ();

	int min_res = m_ec->top ();
	
	RUN:
	auto it = m_ec->m_heap.ordered_begin ();
	auto end= m_ec->m_heap.ordered_end ();
	
 	if (it->getValue () > min_res)
 		goto UPDT;
	
	for ( ; it != end; it++) {
		
		if (it->getValue () != min_res) continue;
		
		int group_id = 0;
		for (std::vector<rca::Link> st : m_temp_trees) {
		
			auto link_it = std::find (st.begin (), st.end(), *it);
			
			//prepare to remove
			if (link_it != st.end ()) {
				
				int link_pos = (link_it - st.begin ()); 
				
				//obter corte em ll(x, y)
 				std::vector<int> cut = this->make_cut( group_id, *it);
 				//pegar os links disponíveis no corte de ll (x,y);
 				std::vector<rca::Link> links;
  				this->getAvailableEdges (cut ,*it, links);
				
				if ( !links.empty() ) {
					
					int _new_link = rand() % links.size ();
					/* Criar link contendo árvore (group_id)
					 * a posição do link na árvore (link_pos)
					 * o link em questão (ll)
					 * o link candidato (links[_new_link])
					 */
					std::tuple<int,int,rca::Link,rca::Link> 	
							tuple (group_id, link_pos, *it, links[_new_link]);
					
					replace (tuple);
					goto RUN;
				}
				
			}
			
			group_id++;
		}
		
	}

	UPDT:
	update_trees ();
	
}

void ChenReplaceVisitor::visitByCost ()
{
	//passar árvores de steiner de SteinerTree para
	//lista de arestas
	prepare_trees ();

	int min_res = m_ec->top ();

	auto it_h = m_ec->m_heap.ordered_begin ();
	auto end_h= m_ec->m_heap.ordered_end ();
	
	std::vector<rca::Link> tree;
	for ( ; it_h != end_h; it_h++) {
		rca::Link l = *it_h;
		l.setValue ( m_network->getCost (it_h->getX(), it_h->getY()) );
		tree.push_back (l);
	}
	
	std::sort (tree.begin (), tree.end(), std::greater<rca::Link>());
	
	auto it = tree.begin ();
	auto end = tree.end();
	
	std::vector<std::tuple<int,int,rca::Link,rca::Link>> to_remove;

	for ( ; it != end; it++) {
		
		to_remove.clear ();
		
		int group_id = 0;
		for (std::vector<rca::Link> st : this->m_temp_trees) {
		
			auto link_it = std::find (st.begin (), st.end(), *it);
			
			//prepare to remove
			if (link_it != st.end ()) {
				
				int link_pos = (link_it - st.begin ()); 
				
				//obter corte em ll(x, y)
 				std::vector<int> cut = this->make_cut( group_id, *it);
 				//pegar os links disponíveis no corte de ll (x,y);
 				std::vector<rca::Link> links;
  				this->getAvailableEdgesByCost (cut ,*it, links);
				
				if ( !links.empty() ) {
					
					int _new_link = rand() % links.size ();
					/* Criar link contendo árvore (group_id)
					 * a posição do link na árvore (link_pos)
					 * o link em questão (ll)
					 * o link candidato (links[_new_link])
					 */
					std::tuple<int,int,rca::Link,rca::Link> 	
							tuple (group_id, link_pos, *it, links[_new_link]);
					
					to_remove.push_back (tuple);

				}
				
			}
			
			group_id++;
		}
		
		for (auto t : to_remove) {
			rca::Link l = std::get<3>(t);
			if (!m_ec->is_used(l)) {
				replace (t);
			} else {
			
				if (m_ec->value (l) > min_res + 1) {
					replace (t);
				} else {					
					int group = std::get<0>(t);					
					auto link_it = std::find (this->m_temp_trees[group].begin (), 
											  this->m_temp_trees[group].end (), l);
					
					if (link_it != this->m_temp_trees[group].end()) {
						int pos = (link_it -  this->m_temp_trees[group].begin ());
						//obter corte em ll(x, y)
						std::vector<int> cut = this->make_cut( group, l);
						//pegar os links disponíveis no corte de ll (x,y);
						std::vector<rca::Link> links;
						this->getAvailableEdgesByCost (cut ,l, links);
						if (!links.empty()) {
							int _new_link = rand() % links.size ();
							/* Criar link contendo árvore (group_id)
							* a posição do link na árvore (link_pos)
							* o link em questão (ll)
							* o link candidato (links[_new_link])
							*/
							std::tuple<int,int,rca::Link,rca::Link> 	
								tt (group, pos, l, links[_new_link]);
							replace (tt);
						}
					}
					
				}
			}
		}
		
	}

	update_trees ();
}

std::vector<int> ChenReplaceVisitor::make_cut (int tree_id, const rca::Link & link)
{
	
	std::vector<int> _int;
	//union_find operations
	int NODES = m_network->getNumberNodes ();
	//marca as duas subárvores geradas
	std::vector<int> nodes_mark = std::vector<int> (NODES,-1);
	
	//estruturas auxiliares
	//filas usadas no processamento
	std::queue<int> nodes_x;
	std::queue<int> nodes_y;
	
	int x = link.getX();
	int y = link.getY();
	nodes_x.push ( x );
	nodes_y.push ( y );
	
	nodes_mark[x] = x;
	nodes_mark[y] = y;
	
	while (!nodes_x.empty () || !nodes_y.empty() ) {
		
		if (!nodes_x.empty())
			x = nodes_x.front ();
		
		if (!nodes_y.empty())
			y = nodes_y.front ();
		
		//while (it != NULL) {
		for (const rca::Link & it : m_temp_trees[tree_id]) {
			//se it.getX() é igula a x, então verifca se a outra
			//aresta foi processada
			if (!nodes_x.empty()) {
				if ( (it.getX() == x) && (nodes_mark[it.getY()] == -1) ) {
					//senão adicione-a a lisda de marcados e guarde 
					//para processamento
					nodes_mark[it.getY()] = nodes_mark[x];
					nodes_x.push (it.getY());
					
					//faz o mesmo para o caso da aresta se it.getY()
				} else if ( (it.getY() == x) && (nodes_mark[it.getX()] == -1) ) {
					nodes_mark[it.getX()] = nodes_mark[x];
					nodes_x.push (it.getX());
				}
			}
			 
			 //processo semelhante para o nó y
			 if (!nodes_y.empty()) {
					if (it.getX() == y && nodes_mark[it.getY()] == -1) {
						nodes_mark[it.getY()] = nodes_mark[y];
						nodes_y.push (it.getY());
					} else if (it.getY() == y && nodes_mark[it.getX()] == -1) {
						nodes_mark[it.getX()] = nodes_mark[y];
						nodes_y.push (it.getX());
					}
			 }
		
		}/*END OF WHILE*/
		
		//remove nó processado
		if (!nodes_x.empty())
			nodes_x.pop ();
		
		if (!nodes_y.empty())
			nodes_y.pop ();
	}
	
	
	return nodes_mark;
}

void ChenReplaceVisitor::replace (TupleEdgeRemove & tuple)
{
	
	int st = std::get<0>(tuple); //getting st tree
	int pos = std::get<1>(tuple);
	rca::Link _old = std::get<2>(tuple);
	rca::Link _new = std::get<3>(tuple);
	
#ifdef DEBUG1
	std::cout << "Replacing " << _old << " by " << _new << std::endl;
#endif 
	
	//updating the tree
	m_temp_trees[st][pos] = _new;
	
	//m_ec->increase (_old,1);
	int value = m_ec->value (_old) + 1;
	
	if (m_ec->is_used (_new)) {
		int v = m_ec->value (_new);
#ifdef DEBUG1
	std::cout << "\t _new value (" << v << ")" << std::endl;
#endif
		m_ec->erase (_new);
		_new.setValue (v - 1);
		m_ec->push (_new);
	} else {
		_new.setValue ( m_network->getBand(_new.getX(), _new.getY()) -1 );
		m_ec->push (_new);
	}
	
	if (value == m_groups.size ()) {
		m_ec->erase (_old);
	} else {
		m_ec->erase (_old);
		_old.setValue (value);
		m_ec->push (_old);
	}
	
#ifdef DEBUG1
	std::cout << "\tMin (" << m_ec->top () << ")\n";
	std::cout << "\tresidual _old (" << _old << ")=" << value;
	std::cout << "\n\tresidual _new (" << _new << ")=" << _new.getValue () << std::endl;
#endif
	
}

void ChenReplaceVisitor::getAvailableEdges(std::vector<int> &cut_xy, 
											const rca::Link & _old,
										   std::vector<rca::Link> & newedges)
{
	
	//guarda arestas que podem subustituir o Link link	
	//árvore com nó x = link.getX
	std::vector<int> Tx;
	//árvore com nó x = link.getY
	std::vector<int> Ty;
	
	//separando árvore em árvore-x e árvore-y
	for (size_t i = 0; i < cut_xy.size (); i++) {
		if (cut_xy[i] == _old.getX ()) {
			Tx.push_back (i);
		} else if (cut_xy[i] == _old.getY ()) {
			Ty.push_back (i);
		}
	}
	
	/*valor de capacidade residual*/
	int residual_cap = m_ec->m_heap.ordered_begin ()->getValue ();
	
	//separando as arestas no corte entre x e y
	for (size_t i = 0; i < Tx.size (); i++) {
		for (size_t j = 0; j < Ty.size (); j++) {
			
			rca::Link l ( Tx[i], Ty[j], 0);
			
			//testa se a aresta existe
			if ( m_network->getCost ( l.getX() , l.getY() ) > 0 && !m_network->isRemoved(l) ) {
				
				//pegar o valor de uso atual
				if ( m_ec->is_used(l) ) {
				
					int usage = m_ec->value (l);
					
					if (usage >  (residual_cap+2) ) {
						if (l != _old) {
							//l.setValue (usage);
							newedges.push_back ( l );
						}
					}
				} else {
					//se não for utilizado ainda, GOOD!
					//l.setValue ( m_groups.size () );
					newedges.push_back (l);
				}				
			}
		}
	}
	
}

void ChenReplaceVisitor::getAvailableEdgesByCost (std::vector<int> &cut_xy, 
								  const rca::Link& _old,
							   std::vector<rca::Link>& newedges)
{
	//guarda arestas que podem subustituir o Link link	
	//árvore com nó x = link.getX
	std::vector<int> Tx;
	//árvore com nó x = link.getY
	std::vector<int> Ty;
	
	//separando árvore em árvore-x e árvore-y
	for (size_t i = 0; i < cut_xy.size (); i++) {
		if (cut_xy[i] == _old.getX ()) {
			Tx.push_back (i);
		} else if (cut_xy[i] == _old.getY ()) {
			Ty.push_back (i);
		}
	}
	
	/*valor de capacidade residual*/
	int residual_cap = m_ec->m_heap.ordered_begin ()->getValue ();
	
	//separando as arestas no corte entre x e y
	for (size_t i = 0; i < Tx.size (); i++) {
		for (size_t j = 0; j < Ty.size (); j++) {
			
			rca::Link l ( Tx[i], Ty[j], 0);
			
			//testa se a aresta existe e se o custo pe menor
			int new_cost = m_network->getCost ( l.getX() , l.getY() );
			int old_cost = m_network->getCost ( _old.getX() , _old.getY() );
			if ( new_cost > 0 && new_cost < old_cost && !m_network->isRemoved(l)) {
				
				//pegar o valor de uso atual
				if ( m_ec->is_used(l) ) {
				
					int usage = m_ec->value (l);
					
					if (usage >=  (residual_cap+1) ) {
						if (l != _old) {
							//l.setValue (usage);
							newedges.push_back ( l );
						}
					}
				} else {
					//se não for utilizado ainda, GOOD!
					//l.setValue ( m_groups.size () );
					newedges.push_back (l);
				}				
			}
		}
	}
}

std::tuple<int,int,rca::Link,rca::Link> 
ChenReplaceVisitor::get_tuple (int group_id, rca::Link& _old)
{
	std::tuple<int,int,rca::Link,rca::Link> tuple;
	
	auto link_it = std::find (this->m_temp_trees[group_id].begin (), 
							  this->m_temp_trees[group_id].end (), _old);
					
	if (link_it != this->m_temp_trees[group_id].end()) {
		
		int pos = (link_it -  this->m_temp_trees[group_id].begin ());
		
		//obter corte em ll(x, y)
		std::vector<int> cut = this->make_cut( group_id , _old );
		
		//pegar os links disponíveis no corte de ll (x,y);
		std::vector<rca::Link> links;		
		this->getAvailableEdgesByCost (cut ,_old, links);
		
		if (!links.empty()) {
			
			int _new_link = rand() % links.size ();
			/* Criar link contendo árvore (group_id)
			* a posição do link na árvore (link_pos)
			* o link em questão (ll)
			* o link candidato (links[_new_link])
			*/
			
			tuple = std::tuple<int,int,rca::Link,rca::Link> 	
					(group_id, pos, _old, links[_new_link]);
			
		}
	}
	return tuple;
}

void ChenReplaceVisitor::update_trees () 
{
	int BAND = m_groups.size ();
	int g = 0;
	m_trees->clear ();
	for (auto st : m_temp_trees) {
	
		STTree _st (m_network->getNumberNodes (), 
						m_groups[g].getSource(), 
						m_groups[g].getMembers ());
		
		
		for (auto link : st) {
			
			int x = link.getX();
			int y = link.getY();
			_st.add_edge (x, y, (int)m_network->getCost (x,y));
			
		}
		
		//_st.prunning ();
		prunning<rca::EdgeContainer<rca::Comparator, rca::HCell>>(_st, *m_ec, 1, BAND);
		
		m_trees->push_back (_st);
		g++;
	}
	
}