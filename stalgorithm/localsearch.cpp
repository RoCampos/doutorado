#include "localsearch.h"

using std::cout;
using std::endl;

LocalSearch::LocalSearch (Network & net, std::vector<rca::Group> & groups)  { 	
	// cout << __FUNCTION__ << endl;
	m_network = &net;
	m_groups = &groups;

}

void LocalSearch::apply (Solution & solution, int & cost, int & res) {	

	this->update_container2 (solution);

	auto begin = to_replace.begin ();
	auto end = to_replace.end ();

	for (; begin != end; begin++) {

		if (this->find(*begin, EdgeType::IN ) 
		|| this->find(*begin, EdgeType::OUT )) 
		continue;		

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

void LocalSearch::do_replace (Solution & solution, int& cost) {

	// cout << __FILE__ << ":" << __FUNCTION__ << endl;
	// cout << "Nº Changes: " << m_id.size () << endl;

	int LINKS = m_id.size ();	

	
	int count = 0;
	for (int i=0; i < LINKS; i++) {

		int tree_id = m_id[i];
		int x = m_out[i].getX();
		int y = m_out[i].getY();

		if ( !solution[tree_id].find_edge (x,y) ) {
			// cout << "NotFound: " << m_out[i] << endl;
			continue;
		}

		// cout << m_out[i] << " " << m_out[i].getValue() <<" | ";
		// cout << m_in[i] << " " << m_in[i].getValue() << " -- ";
		// cout << m_id[i] << endl;

		// count += ( (int)m_out[i].getValue() - (int)m_in[i].getValue());

		cost -= solution[tree_id].get_cost ();
		
		//removendo e atualizando custo
		solution[tree_id].remove_edge (x, y);

		int tree_cost = solution[tree_id].get_cost () - (int) m_out[i].getValue ();

		solution[tree_id].set_cost (tree_cost);

		x = m_in[i].getX();
		y = m_in[i].getY();
		solution[tree_id].add_edge ( x, y, (int) m_in[i].getValue ());

		//make prunning and getting cost of removed edges
		Prune p;
		p.prunning (solution[tree_id]);
		int removedcost = 0;
		for (auto e = p.begin(); e != p.end(); e++) {
			// cout << "\t" << e->first << "--" << e->second << " ";
			// cout << (int) m_network->getCost (e->first, e->second) << endl;
			removedcost += (int)m_network->getCost (e->first, e->second);													
		}

		count += removedcost;

		//updating cost
		tree_cost = solution[tree_id].get_cost () - removedcost;
		solution[tree_id].set_cost (tree_cost);

		cost += solution[tree_id].get_cost ();
	}

	// this->m_id.clear ();
	// this->m_out.clear ();
	// this->m_in.clear ();

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

