#include "mpp_visitor.h"

using namespace rca;
using namespace rca::sttalgo;

template<class SolutionType>
void ChenReplaceVisitor<SolutionType>::visit ()
{
#ifdef DEBUG
	std::cout <<"----------"<<__FUNCTION__ <<"-------------"<< std::endl;
#endif
	
	//passar árvores de steiner de SteinerTree para
	//lista de arestas
	this->prepare_trees ();

	int min_res = m_ec->top ();
	
	int size_of_list = 0;
	
	RUN:
	auto it = m_ec->m_heap.ordered_begin ();
	auto end= m_ec->m_heap.ordered_end ();
	
 	if (it->getValue () > min_res) {
		
#ifdef DEBUG
	std::cout << "Possible Loop" << std::endl;
#endif
		goto UPDT;
	} else {
		
 		if (size_of_list < this->m_replaced.size ()) {
 			size_of_list = this->m_replaced.size ();
 		} else {
 			goto UPDT;
 		}
	}
	
	for ( ; it != end; it++) {
		
		if (it->getValue () != min_res) continue;
		
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
  				this->getAvailableEdges (cut ,*it, links, group_id);
				
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
	
#ifdef DEBUG
	std::cout <<"-----endof "<<__FUNCTION__ <<"-------------"<< std::endl;
#endif
	
}

template<class SolutionType>
void ChenReplaceVisitor<SolutionType>::visitByCost ()
{
#ifdef DEBUG1
	std::cout <<"----------"<<__FUNCTION__ <<"-------------" << std::endl;
#endif
	
	//passar árvores de steiner de SteinerTree para
	//lista de arestas
	this->prepare_trees ();

	int min_res = m_ec->top ();

	auto it_h = m_ec->m_heap.ordered_begin ();
	auto end_h= m_ec->m_heap.ordered_end ();
	
	std::vector<rca::Link> tree;
	for ( ; it_h != end_h; it_h++) {
		rca::Link l = *it_h;
		l.setValue ( this->m_network->getCost (it_h->getX(), it_h->getY()) );
		tree.push_back (l);
	}
	
	std::sort (tree.begin (), tree.end(), std::greater<rca::Link>());
	
	auto it = tree.begin ();
	auto end = tree.end();
	
	std::vector<std::tuple<int,int,rca::Link,rca::Link>> to_remove;

	std::vector<rca::Link> marked;
	
	for ( ; it != end; it++) {
		
		to_remove.clear ();
		
		marked.clear();
		
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
  				this->getAvailableEdgesByCost (cut ,*it, links, group_id);
				
				if ( !links.empty() ) {
					
					int _new_link = rand() % links.size ();
					/* Criar link contendo árvore (group_id)
					 * a posição do link na árvore (link_pos)
					 * o link em questão (ll)
					 * o link candidato (links[_new_link])
					 */
					
					for (auto lll : links) {
					
						auto b = marked.begin();
						auto e = marked.end();	
						if (std::find(b,e,lll) == e) {
							
							std::tuple<int,int,rca::Link,rca::Link> 	
								tuple (group_id, link_pos, *it, lll);					
								
							to_remove.push_back (tuple);					
							break;
						}
						
					}
					
				}
				
			}
			
			group_id++;
		}
		
		for (auto t : to_remove) {
			int group = std::get<0>(t);	
			rca::Link l = std::get<3>(t);
			if (!m_ec->is_used(l)) {
				replace (t);
			} else {
			
				// if (m_ec->value (l) > min_res + 1) {
				int trequest = this->m_groups[group].getTrequest ();
				int l_curr_cap = m_ec->value (l);
				if ( (l_curr_cap - trequest) > min_res ) {
					replace (t);				
				} else {					
									
					auto link_it = std::find (this->m_temp_trees[group].begin (), 
											  this->m_temp_trees[group].end (), l);
					
					if (link_it != this->m_temp_trees[group].end()) {
						int pos = (link_it -  this->m_temp_trees[group].begin ());
						//obter corte em ll(x, y)
						std::vector<int> cut = this->make_cut( group, l);
						//pegar os links disponíveis no corte de ll (x,y);
						std::vector<rca::Link> links;
						this->getAvailableEdgesByCost (cut ,l, links, group);
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

template<class SolutionType>
std::vector<int> ChenReplaceVisitor<SolutionType>::make_cut (int tree_id, const rca::Link & link)
{
#ifdef DEBUG1
	std::cout <<"----------"<<__FUNCTION__ <<"-------------"<< std::endl;
#endif
	std::vector<int> _int;
	//union_find operations
	int NODES = this->m_network->getNumberNodes ();
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
		for (const rca::Link & it : this->m_temp_trees[tree_id]) {
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

template<class SolutionType>
void ChenReplaceVisitor<SolutionType>::replace (TupleEdgeRemove & tuple)
{
#ifdef DEBUG
	std::cout <<"----------"<<__FUNCTION__ <<"-------------"<< std::endl;
#endif	
	int st = std::get<0>(tuple); //getting st tree
	int pos = std::get<1>(tuple);
	rca::Link _old = std::get<2>(tuple);
	rca::Link _new = std::get<3>(tuple);
	
#ifdef DEBUG1
	std::cout << "Replacing " << _old << " by " << _new << std::endl;
#endif 
	
	//updating the tree
	this->m_temp_trees[st][pos] = _new;
	
	//getting the band needed to group st
	int trequest = this->m_groups[st].getTrequest ();

	//max_band of _old link
	int old_band = this->m_network->getBand (_old.getX(), _old.getY());

	//current capacity of _old_link
	int old_capacity = m_ec->value (_old) + trequest;
	
	if (m_ec->is_used (_new)) {
		int v = m_ec->value (_new);
#ifdef DEBUG1
	std::cout << "\t _new value (" << v << ")" << std::endl;
#endif
		m_ec->erase (_new);
		_new.setValue (v - trequest);
		m_ec->push (_new);
	} else {
		int band = this->m_network->getBand(_new.getX(), _new.getY());
		_new.setValue ( band - trequest  );
		m_ec->push (_new);
	}
	
	if ( old_capacity == old_band ) {
		m_ec->erase (_old);
	} else {
		m_ec->erase (_old);
		_old.setValue (old_capacity);
		m_ec->push (_old);
	}
	
#ifdef DEBUG1
	std::cout << "\tMin (" << m_ec->top () << ")\n";
	std::cout << "\tresidual _old (" << _old << ")=" << value;
	std::cout << "\n\tresidual _new (" << _new << ")=" << _new.getValue () << std::endl;
#endif
	
	this->push_replaced (_old);
}

template<class SolutionType>
void ChenReplaceVisitor<SolutionType>::getAvailableEdges(std::vector<int> &cut_xy, 
											const rca::Link & _old,
										   std::vector<rca::Link> & newedges, int tree_id)
{
#ifdef DEBUG1
	std::cout <<"----------"<<__FUNCTION__ <<"-------------"<< std::endl;
#endif	
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
	int min_capacity = m_ec->m_heap.ordered_begin ()->getValue ();
	int trequest = this->m_groups[tree_id].getTrequest ();
	
	//separando as arestas no corte entre x e y
	for (size_t i = 0; i < Tx.size (); i++) {
		for (size_t j = 0; j < Ty.size (); j++) {
			
			rca::Link l ( Tx[i], Ty[j], 0);
			
			//testa se a aresta existe
			if ( this->m_network->getCost ( l.getX() , l.getY() ) > 0 && !this->m_network->isRemoved(l) ) {
				
				//pegar o valor de uso atual
				if ( m_ec->is_used(l) ) {
				
					// getting current capacity of link l
					int curr_capacity = m_ec->value (l);
					
					// getting current capacity of link _old 
					int old_capacity = this->m_network->getBand (
						_old.getX(),_old.getY());

					// check if the new link l can replace linn _old
					if ( (curr_capacity - trequest) > old_capacity) {

						if (l != _old)
							newedges.push_back (l);

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

template<class SolutionType>
void ChenReplaceVisitor<SolutionType>::getAvailableEdgesByCost (std::vector<int> &cut_xy, 
								  const rca::Link& _old,
							   std::vector<rca::Link>& newedges,
							   int tree_id)
{
#ifdef DEBUG1
	std::cout <<"----------"<<__FUNCTION__ <<"-------------"<< std::endl;
#endif
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
	int min_capacity = m_ec->m_heap.ordered_begin ()->getValue ();
	int trequest = this->m_groups[tree_id].getTrequest ();
	
	//separando as arestas no corte entre x e y
	for (size_t i = 0; i < Tx.size (); i++) {
		for (size_t j = 0; j < Ty.size (); j++) {
			
			rca::Link l ( Tx[i], Ty[j], 0);
			
			//testa se a aresta existe e se o custo pe menor
			int new_cost = this->m_network->getCost ( l.getX() , l.getY() );
			int old_cost = this->m_network->getCost ( _old.getX() , _old.getY() );
			if ( new_cost > 0 && new_cost < old_cost && !this->m_network->isRemoved(l)) {
				
				//pegar o valor de uso atual
				if ( m_ec->is_used(l) ) {
				
					// getting current capacity of link l
					int curr_capacity = m_ec->value (l);
					
					// getting current capacity of link _old 
					int old_capacity = this->m_network->getBand (
						_old.getX(),_old.getY());

					// check if the new link l can replace linn _old
					if ( (curr_capacity - trequest) > old_capacity) {

						if (l != _old)
							newedges.push_back (l);

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

template<class SolutionType>
std::tuple<int,int,rca::Link,rca::Link> 
ChenReplaceVisitor<SolutionType>::get_tuple (int group_id, rca::Link& _old)
{
#ifdef DEBUG1
	std::cout <<"----------"<<__FUNCTION__ <<"-------------"<< std::endl;
#endif
	std::tuple<int,int,rca::Link,rca::Link> tuple;
	
	auto link_it = std::find (this->m_temp_trees[group_id].begin (), 
							  this->m_temp_trees[group_id].end (), _old);
					
	if (link_it != this->m_temp_trees[group_id].end()) {
		
		int pos = (link_it -  this->m_temp_trees[group_id].begin ());
		
		//obter corte em ll(x, y)
		std::vector<int> cut = this->make_cut( group_id , _old );
		
		//pegar os links disponíveis no corte de ll (x,y);
		std::vector<rca::Link> links;		
		this->getAvailableEdgesByCost (cut ,_old, links, group_id);
		
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

template<class SolutionType>
void ChenReplaceVisitor<SolutionType>::update_trees () 
{
#ifdef DEBUG1
	std::cout <<"----------"<<__FUNCTION__ <<"-------------"<< std::endl;
#endif
	this->m_cost = 0;
	
	int BAND = this->m_groups.size ();
	int g = 0;
	this->m_trees->clear ();
	for (auto st : this->m_temp_trees) {
	
		SolutionType _st (this->m_network->getNumberNodes (), 
						this->m_groups[g].getSource(), 
						this->m_groups[g].getMembers ());
		
		
		for (auto link : st) {
			
			int x = link.getX();
			int y = link.getY();
			_st.add_edge (x, y, (int)this->m_network->getCost (x,y));
			
		}
		
		//_st.prunning ();
		int trequest = this->m_groups[g].getTrequest ();		

		prunning<rca::EdgeContainer<rca::Comparator, rca::HCell>, SolutionType>
			(_st, *m_ec, trequest, BAND, *this->m_network);
		
		this->m_cost += _st.get_cost ();
		
		this->m_trees->push_back (_st);
		g++;
	}
	
}


template class rca::sttalgo::ChenReplaceVisitor<STTree>;
template class rca::sttalgo::MppVisitor<STTree>;
template class rca::sttalgo::ChenReplaceVisitor<steiner>;
template class rca::sttalgo::MppVisitor<steiner>;

