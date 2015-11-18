#include "localsearch.h"

using std::cout;
using std::endl;

LocalSearch::LocalSearch (Network & net, std::vector<rca::Group> & groups)  { 	
	// cout << __FUNCTION__ << endl;
	m_network = &net;
	m_groups = &groups;

}

void LocalSearch::apply (Solution & solution, int & cost, int & res) {	

	this->update_container (solution);

	auto begin = to_replace.begin ();
	auto end = to_replace.end ();

	for (; begin != end; begin++) {		

		int id = 0;
		bool hasFound = false;
		for (steiner & tree : solution) {						
			if ( this->cut_replace (begin->getX(), begin->getY(), id, tree, cost) ) {					
				break;
			}
			id++;
		}		
	}

	// for (auto l : this->m_removed ) {
	// 	cout << l << endl;		
	// }
	this->m_removed.clear ();
	// cout << cost << "**" << m_cg->top () << endl;

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

	// cout << "ToReplace :" << old << " " << old.getValue() << endl;

	if (tree.find_edge (x, y)) {
		int origcost = tree.get_cost ();

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

						// cout << "\t" << out <<" "<< out.getValue()<< endl;
						if (in.getValue () < old.getValue()) {
							
							int value;
							if (m_cg->is_used (in)) {
								value = m_cg->value (in);
							} else {
								value = m_network->getBand (in.getX(), in.getY());
							}

							int tk = m_groups->at (id).getTrequest ();

							if ( (value - tk) > 0 && (value-tk) > TOP ) {
								
								// m_out.push_back (old);
								// m_in.push_back (in);
								// m_id.push_back (id);

								// tree.add_edge (x, y, 0);

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

