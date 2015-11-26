#include "localsearch.h"

using std::cout;
using std::endl;

using namespace rca::sttalgo;

LocalSearch::LocalSearch (Network & net, std::vector<rca::Group> & groups)  { 	
	// cout << __FUNCTION__ << endl;
	m_network = &net;
	m_groups = &groups;

}

LocalSearch::LocalSearch (Network & net, std::vector<rca::Group> & groups, Container & cont) {

	m_network = &net;
	m_groups = &groups;
	m_cg = &cont;

}

void LocalSearch::apply (Solution & solution, int & cost, int & res) {	

#ifdef debug1
	cout << "LocalSearch:" <<__FUNCTION__ << endl;
#endif

	// this->update_container (solution);

	std::vector<rca::Link> list;
	for (auto b = this->m_cg->m_heap.ordered_begin(); 
		b != this->m_cg->m_heap.ordered_end(); b++) {
		int bcost = (int) m_network->getCost (b->getX(), b->getY());
		rca::Link l (b->getX(), b->getY(), bcost);
		list.push_back (l);
	}

	auto begin = to_replace.begin ();
	auto end = to_replace.end ();
	
	for (; begin != end; begin++) {		

		int id = 0;
		// bool hasFound = false;

		std::vector<int> gindex (m_groups->size ());
		std::iota (gindex.begin(), gindex.end(), 0);
		std::random_shuffle (gindex.begin(), gindex.end());

		// for (steiner & tree : solution) {						
		for (auto id : gindex) {
			// if ( this->cut_replace (begin->getX(), begin->getY(), id, tree, cost) ) {					
			if ( this->cut_replace (begin->getX(), begin->getY(), id, solution[id], cost) ) {					
				break;
			}
			// id++;
		}		
	}
	
	this->m_removed.clear ();

	res = m_cg->top ();
}

void LocalSearch::inline_replace (steiner & solution, 
	int& cost, rca::Link& out, rca::Link& in, int tree_id) {

	cost -= solution.get_cost ();

	int outcost = (int) m_network->getCost (out.getX(), out.getY());
	int incost = (int) m_network->getCost (in.getX(), in.getY());

	//remove aresta e atualiza o custo
	solution.remove_edge (out.getX(), out.getY());
	solution.set_cost ( solution.get_cost () - outcost);

	//adicionando nova aresta, custo atualizado
	solution.add_edge (in.getY(), in.getX(), incost);

	//Prune nós com grau que não são terminais
	Prune p;
	p.prunning (solution);
	int removedcost = 0;
	for (auto e = p.begin(); e != p.end(); e++) {		
		removedcost += (int)m_network->getCost (e->first, e->second);
		//atualizando capacidade residual
		rca::Link l (e->first, e->second, 0);
		this->m_removed.push_back (l);

		this->inline_update (l, EdgeType::OUT, tree_id);

	}
	this->m_removed.push_back (out);

	//remove custo das arestas prunneds
	solution.set_cost (solution.get_cost () - removedcost);
	
	cost += solution.get_cost ();

}

bool LocalSearch::cut_replace (int x, int y, int id, steiner & tree, int& solcost) {

	// cout << __FUNCTION__ << endl;

	int oldvalue = (int) this->m_network->getCost (x,y);
	rca::Link old (x, y, oldvalue);

	if (tree.find_edge (x, y)) {
		// int origcost = tree.get_cost ();

		tree.remove_edge (x, y);

		//getting conected components
		CC cc (tree);		
		std::vector<std::vector<int>> ccs;

		//creating component list
		cc.get_components_list (ccs);

		if (ccs.size () < 2) {
			tree.add_edge (x, y, 0);
			return false;
		}

		//adicionando o link novamente
		tree.add_edge (x, y, 0);

		int TOP = m_cg->top ();
		
		for (auto i : ccs[0]) {
			for (auto j : ccs[1]) {
				int cost = (int)this->m_network->getCost(i,j);
				rca::Link in (i,j, cost);
				if (in.getValue () > 0 ) { //existe
					if (in != old) { //não é o mesmo	

						if (in.getValue () < old.getValue()) {
							
							int value;
							if (m_cg->is_used (in)) {
								value = m_cg->value (in);
							} else {
								value = m_network->getBand (in.getX(), in.getY());
							}

							int tk = m_groups->at (id).getTrequest ();

							if ( (value - tk) > 0 && (value-tk) >= TOP ) {
								
								inline_replace (tree, solcost ,old, in, id);

								inline_update (old, EdgeType::OUT, id);
								inline_update (in, EdgeType::IN, id);
								
								return true;
							}
						}
					}
				}
			}
		}

		return false;
	}

	return false;

}

void LocalSearch::inline_update (rca::Link& out, EdgeType type, int tree_id) {

	//obter requisição de tráfego
	int tk = (int)this->m_groups->at(tree_id).getTrequest ();	
	
	//pegando a banda da aresta
	int BAND = (int)m_network->getBand (out.getX(), out.getY());

	//liberando banda ou consumindo banda
	//OUT e IN são usados para determinar a estrategia
	int value;
	if (EdgeType::OUT == type) {		
		value = m_cg->value (out) + tk;		
	} else if (EdgeType::IN == type) {		
		if (m_cg->is_used (out)) {
			value = m_cg->value (out) - tk;
		} else {
			value = BAND - tk;
		} 
	}


	if (value ==  BAND) {
		m_cg->erase (out);
	} else {
		if (m_cg->is_used (out)) {
			m_cg->erase (out);
			out.setValue (value);
		} else {
			out.setValue (value);
		}
		m_cg->push (out);
	}
	//atualizando a banda
	
}

void LocalSearch::update_container (Solution & solution) {

	m_cg = new Container;
	m_cg->init_handle_matrix ( m_network->getNumberNodes () );

	int LINKS = solution.size ();

	for (auto & tree : solution) {
		auto edges = tree.get_all_edges ();			
		for (const std::pair<int,int> & e : edges) {
			int cost = m_network->getCost (e.first, e.second);
			rca::Link l (e.first, e.second, cost);

			if ( !m_cg->is_used (l) ) {					

				to_replace.push_back (l);

				l.setValue (LINKS-1);
				m_cg->push (l);

			} else {
				int value = m_cg->value (l);
				m_cg->erase (l);
				l.setValue (value - 1);
				m_cg->push (l);
			}
		}
	}

	std::sort (to_replace.begin(), to_replace.end(), std::greater<rca::Link>());
}


// --------------------------- CycleLocalSearch ---------------------------------- 

CycleLocalSearch::CycleLocalSearch (Network & net, std::vector<rca::Group> & groups) {

	m_network = &net;
	m_groups = &groups;
	m_cg = NULL;
}

CycleLocalSearch::CycleLocalSearch (Network & net, std::vector<rca::Group> & groups, Container& cont) {
	m_network = &net;
	m_groups = &groups;
	m_cg = &cont;
}

CycleLocalSearch::~CycleLocalSearch () {
	// delete m_cg;
	m_cg = NULL;
	m_network = NULL;
	m_groups = NULL;
}

void CycleLocalSearch::update_container (Solution & solution) {

	m_cg = new Container;
	m_cg->init_handle_matrix ( m_network->getNumberNodes () );

	int LINKS = solution.size ();

	for (auto & tree : solution) {
		auto edges = tree.get_all_edges ();			
		for (const std::pair<int,int> & e : edges) {
			int cost = m_network->getCost (e.first, e.second);
			rca::Link l (e.first, e.second, cost);

			if ( !m_cg->is_used (l) ) {					

				to_replace.push_back (l);

				l.setValue (LINKS-1);
				m_cg->push (l);

			} else {
				int value = m_cg->value (l);
				m_cg->erase (l);
				l.setValue (value - 1);
				m_cg->push (l);
			}
		}
	}

	std::sort (to_replace.begin(), to_replace.end(), std::greater<rca::Link>());
}

void CycleLocalSearch::apply (Solution & solution, int & cost, int & res) {

#ifdef debug1
	cout << "CycleLocalSearch:" <<__FUNCTION__ << endl;
#endif

	// this->update_container (solution);

	int tree_id = 0;
	for (steiner & st : solution) { //O(K)

		//getting the vertex of the tree
		std::vector<int> vertex;
		this->get_vertex (vertex, st);
		this->cut_replace (tree_id, vertex, st, cost);

		//inline_replace (vertex, st);
		
		tree_id++;
	}

}

void CycleLocalSearch::get_vertex (std::vector<int> & vertex, 
	steiner & st) {

	int NODES = m_network->getNumberNodes ();
	for (int i=0; i < NODES; i++) { //O(V)
		if (st.get_degree(i) > 0) {
			vertex.push_back (i);
		}
	}	

}

bool CycleLocalSearch::cut_replace (int id, std::vector<int>& vertex, steiner & st, int& solcost) {


	int top = m_cg->top ();

	//V*V*O(V+E) --> O(V³ + E) 
	for (auto x : vertex) {
		for (auto y : vertex) {
			if (x < y) {

				rca::Link in (x,y, 0);
				int in_cost = m_network->getCost (x,y);

				in.setValue (in_cost);

				if ( in_cost > 0 && !st.find_edge (x,y) ) {

					//avitar piorar solução
					if (m_cg->is_used (in) && m_cg->value (in) <= top) {
						continue;
					}

					rca::Path p = this->get_path (x,y,st);

					if (p.size () == 0) {
						continue;
					} 

					rca::Link out = this->get_out_link (p, st);

					if (out.getX() != out.getY()) {
						
						int solcost_copy = solcost;
						steiner st_copy = st;
						inline_replace (st_copy, solcost_copy ,out, in, id);
						
						if (solcost_copy < solcost) {
							
							solcost = solcost_copy;
							st = st_copy;
							inline_update (out, EdgeType::OUT, id);
							inline_update (in, EdgeType::IN, id);

							//remover arestas do prunning
							// std::cout << "\ttoremove\n";
							for (auto & e : this->m_removed) {
								// cout << e << endl;
								inline_update (e, EdgeType::OUT, id);
							}
						}

						this->m_removed.clear ();

					}

				}

			}

		}
	}

	return false;

}

rca::Link CycleLocalSearch::get_out_link (rca::Path& path, steiner & st) {

	for (auto it = path.begin (); it != path.end () - 1; it++) {

		rca::Link l (*it, *(it+1), 0);
		int cost = this->m_network->getCost (l.getX(), l.getY());
		l.setValue (cost);
		
		if ( !st.is_terminal (l.getX()) && !st.is_terminal (l.getY()) ) {
			return l;
		}		

	}

	rca::Link l(0,0,0);
	return l;
}

void CycleLocalSearch::inline_replace (steiner & solution, 
	int& cost, rca::Link& out, rca::Link& in, int tree_id) {

	cost -= solution.get_cost ();

	int outcost = (int) m_network->getCost (out.getX(), out.getY());
	int incost = (int) m_network->getCost (in.getX(), in.getY());

	//remove aresta e atualiza o custo
	solution.remove_edge (out.getX(), out.getY());
	solution.set_cost ( solution.get_cost () - outcost);

	//adicionando nova aresta, custo atualizado
	solution.add_edge (in.getY(), in.getX(), incost);

	//Prune nós com grau que não são terminais
	Prune p;
	p.prunning (solution);
	int removedcost = 0;
	for (auto e = p.begin(); e != p.end(); e++) {		
		removedcost += (int)m_network->getCost (e->first, e->second);
		//atualizando capacidade residual
		rca::Link l (e->first, e->second, 0);
		this->m_removed.push_back (l);		
	}	

	//remove custo das arestas prunneds
	solution.set_cost (solution.get_cost () - removedcost);
	
	cost += solution.get_cost ();

}

void CycleLocalSearch::inline_update (rca::Link& out, EdgeType type, int tree_id) {

	//obter requisição de tráfego
	int tk = (int)this->m_groups->at(tree_id).getTrequest ();	
	
	//pegando a banda da aresta
	int BAND = (int)m_network->getBand (out.getX(), out.getY());

	//liberando banda ou consumindo banda
	//OUT e IN são usados para determinar a estrategia
	int value;
	if (EdgeType::OUT == type) {		
		value = m_cg->value (out) + tk;
	} else if (EdgeType::IN == type) {		
		if (m_cg->is_used (out)) {
			value = m_cg->value (out) - tk;
		} else {
			value = BAND - tk;
		} 
	}


	if (value ==  BAND) {
		m_cg->erase (out);
	} else {
		if (m_cg->is_used (out)) {
			m_cg->erase (out);
			out.setValue (value);
		} else {
			out.setValue (value);
		}
		m_cg->push (out);
	}
	//atualizando a banda
	
}