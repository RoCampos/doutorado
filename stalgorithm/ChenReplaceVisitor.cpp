#include "mpp_visitor.h"


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
	
	for ( ; it != end && it->getValue () == min_res; it++) {
		
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
					
					//guardar para remoção
					TupleEdgeRemove toRemove;
					toRemove.push_back (tuple);
					
					replace (toRemove);
					goto RUN;
				}
				
			}
			
			group_id++;
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

void ChenReplaceVisitor::replace (TupleEdgeRemove & tuples)
{
	
	for (auto it : tuples) {
		
		int st = std::get<0>(it); //getting st tree
		int pos = std::get<1>(it);
		rca::Link _old = std::get<2>(it);
		rca::Link _new = std::get<3>(it);
		
		//updating the tree
		m_temp_trees[st][pos] = _new;
		
		_old.setValue ( m_ec->value (_old) + 1);
		
		if (m_ec->is_used (_new)) {
			_new.setValue ( m_ec->value (_new) - 1);
			m_ec->update (_new);
		} else {
			_new.setValue ( m_network->getBand(_new.getX(), _new.getY()) -1 );
			m_ec->push (_new);
		}
				
		//updating usage
		m_ec->update (_old);
		
	}
	
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
			if ( m_network->getCost ( l.getX() , l.getY() ) > 0 ) {
				
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

void ChenReplaceVisitor::update_trees () 
{
	
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
		
		_st.prunning ();
		
		m_trees->push_back (_st);
		g++;
	}
	
}