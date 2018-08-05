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

	this->m_top = m_cg->top ();

}

void LocalSearch::apply (Solution & solution, int & cost, int & res) {	

#ifdef debug1
	cout << "LocalSearch:" <<__FUNCTION__ << endl;
#endif

	std::vector<rca::Link> list;
	for (auto b = this->m_cg->m_heap.ordered_begin(); 
		b != this->m_cg->m_heap.ordered_end(); b++) {
		int bcost = (int) m_network->getCost (b->getX(), b->getY());
		rca::Link l (b->getX(), b->getY(), bcost);
		list.push_back (l);
	}

	std::sort (list.begin(), list.end(), std::greater<rca::Link>());

	auto begin = list.begin ();
	auto end = list.end ();
	
	for (; begin != end; begin++) {		

		int id = 0;
		// bool hasFound = false;

		std::vector<int> gindex (m_groups->size ());
		std::iota (gindex.begin(), gindex.end(), 0);
		std::random_shuffle (gindex.begin(), gindex.end());

		for (auto id : gindex) {			
			if ( this->cut_replace (begin->getX(), begin->getY(), id, solution[id], cost) ) {					
				break;
			}
		}		
	}
	
	this->m_removed.clear ();

	res = m_cg->top ();
}

void LocalSearch::inline_replace (steiner & solution, 
	int& cost, rca::Link& out, rca::Link& in, int tree_id) {

	cost -= (int)solution.get_cost ();

	int outcost = (int) m_network->getCost (out.getX(), out.getY());
	int incost = (int) m_network->getCost (in.getX(), in.getY());

	//remove aresta e atualiza o custo
	solution.remove_edge (out.getX(), out.getY());
	solution.set_cost ( (int)solution.get_cost () - outcost);

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
	solution.set_cost ( (int)solution.get_cost () - removedcost);
	
	cost += (int)solution.get_cost ();

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
								value = (int)m_network->getBand (in.getX(), in.getY());
							}

							int tk = (int)m_groups->at (id).getTrequest ();

							if ( (value - tk) > 0 && (value-tk) >= this->m_top ) {
								
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
		value = (m_cg->value (out) + tk);
	} else if (EdgeType::IN == type) {		
		if (m_cg->is_used (out)) {
			value = (m_cg->value (out) - tk);
		} else {
			value = (BAND - tk);
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
			int cost = (int)m_network->getCost (e.first, e.second);
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

	this->m_top = m_cg->top ();
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
			int cost = (int)m_network->getCost (e.first, e.second);
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

	int tree_id = 0;
	for (steiner & st : solution) { //O(K)
		//getting the vertex of the tree
		std::vector<int> vertex;
		this->get_vertex (vertex, st);
		this->cut_replace (tree_id, vertex, st, cost);		
		tree_id++;
	}

}

void CycleLocalSearch::get_vertex (
	std::vector<int> & vertex, 
	steiner & st) {

	int NODES = m_network->getNumberNodes ();
	for (int i=0; i < NODES; i++) { //O(V)
		if (st.get_degree(i) > 0) {
			vertex.push_back (i);
		}
	}	

}

int CycleLocalSearch::contador = 0;

bool CycleLocalSearch::cut_replace (
	int id, 
	std::vector<int>& vertex, 
	steiner & st, 
	int& solcost) {

	CycleLocalSearch::contador++;

	//V*V*O(V+E) --> O(V³ + E) 
	for (auto x : vertex) {
		for (auto y : vertex) {
			if (x < y) {

				rca::Link in (x,y,0);
				int in_cost = (int)m_network->getCost (x,y);

				in.setValue (in_cost);

				if ( in_cost > 0 && !st.find_edge (x,y) ) {

					//avitar piorar solução
					int tk = (int)m_groups->at (id).getTrequest ();
					if (m_cg->is_used (in) && (m_cg->value (in)-tk < this->m_top)) {
						continue;
					} else if ((int)m_network->getBand(x,y)-tk <this->m_top) 
						continue;

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
							for (auto & e : this->m_removed) {								
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

rca::Link CycleLocalSearch::get_out_link (
	rca::Path& path, 
	steiner & st) 
{

	for (auto it = path.begin (); it != path.end () - 1; it++) {
		rca::Link l (*it, *(it+1), 0);
		int cost = (int)this->m_network->getCost (l.getX(), l.getY());
		l.setValue (cost);		
		if ( !st.is_terminal (l.getX()) && !st.is_terminal (l.getY()) ) {
			return l;
		}		
	}
	rca::Link l(0,0,0);
	return l;
}

void CycleLocalSearch::inline_replace (
	steiner & solution, 
	int& cost, 
	rca::Link& out, 
	rca::Link& in, 
	int tree_id) 
{

	cost -= (int)solution.get_cost ();

	int outcost = (int) m_network->getCost (out.getX(), out.getY());
	int incost = (int) m_network->getCost (in.getX(), in.getY());

	//remove aresta e atualiza o custo
	solution.remove_edge (out.getX(), out.getY());
	solution.set_cost ( (int)solution.get_cost () - outcost);

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
	solution.set_cost ( (int)solution.get_cost () - removedcost);
	
	cost += (int)solution.get_cost ();

}

void CycleLocalSearch::inline_update (
	rca::Link& out, 
	EdgeType type, 
	int tree_id) 
{

	//obter requisição de tráfego
	int tk = (int)this->m_groups->at(tree_id).getTrequest ();	
	
	//pegando a banda da aresta
	int BAND = (int)m_network->getBand (out.getX(), out.getY());

	//liberando banda ou consumindo banda
	//OUT e IN são usados para determinar a estrategia
	int value;
	if (EdgeType::OUT == type) {		
		value = (m_cg->value (out) + tk);
	} else if (EdgeType::IN == type) {		
		if (m_cg->is_used (out)) {
			value = (m_cg->value (out) - tk);
		} else {
			value = (BAND - tk);
		} 
	}


	if (value == BAND) {
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

// ----------------- ReplaceEdgeConstrained ------------//

template <class Container, class SteinerRpr>
std::vector<std::vector<int>> 
PathExchange<Container, SteinerRpr>::get_components (
	SteinerRpr& st)
{
	CC cc (st);
	std::vector<std::vector<int>> ccs;
	assert (cc.get_num_cc () > 1);	
	cc.get_components_list (ccs);

	return ccs;
}

template <class Container, class SteinerRpr>
bool PathExchange<Container, SteinerRpr>::test_pathsize (
	int cc1, 
	int cc2, 
	Paths & paths, 
	std::vector<int> distancia)
{
	int dis_c0 = distancia[cc1];	
	for (auto && p : paths) {
		if (p.find_in (cc2)) {
			for (size_t i = 0; i < p.size (); ++i)	{
				if (p[i] == cc2) {
					if ( (p.size () - (i)) + dis_c0 > this->m_path_size ) {						
						return false;
					} 
				}
			}
		} 
	}		
	return true;
}

template <class Container, class SteinerRpr>
bool PathExchange<Container, SteinerRpr>::control_trocas (
	rca::Link& novo, rca::Link& old) {

	auto pair1 = std::make_pair(old, novo);
	auto pair2 = std::make_pair(novo, old);

	auto res1 = std::find (
		m_trocas.begin(), 
		m_trocas.end(), pair1);

	auto res2 = std::find (
		m_trocas.begin(),
		m_trocas.end(), pair2);

	if (res1 != m_trocas.end() || res2 != m_trocas.end())
	{
		return true;
	}

	m_trocas.push_back ( std::make_pair(old, novo) );

	return false;

}

template <class Container, class SteinerRpr>
void PathExchange<Container, SteinerRpr>::paths_distance (
	SteinerRpr& st,  
	Paths& paths, 
	std::vector<int>& distancia, 
	std::vector<int>& members, 
	int source)
{
	for (auto && m: members) {		
		rca::Path path ( st.get_path (source, m) );		
		paths.push_back (path);
		for (size_t i = 0; i < path.size (); i++) {
			distancia[ path[i] ] = i;
		}
	}
}

template <class Container, class SteinerRpr>
std::vector<rca::Path> 
PathExchange<Container, SteinerRpr>::prepare_paths (
	Paths& paths, int connec, int oldroot, int newroot) 
{
	Paths newpaths; // = paths;

	//int pos and the path
	std::vector<std::pair<rca::Path, rca::Path>> v;

	// eliminar a aresta ausente
	rca::Path rootpath;

	//selecionar os caminhos que contem a nova raiz
	//newroot que será conectado a outra parte da árvore que
	//contém a fonte
	for (int i = 0; i < paths.size (); ++i)
	{
		if (paths[i].find_in (oldroot)) {			

			if (paths[i].find_in (newroot)) 
				rootpath = paths[i];

			//creating subpath
			std::vector<int> vec;
			paths[i].subpath (connec, vec);			
			Path tmp (vec);
			tmp.reverse ();
			v.push_back (make_pair (paths[i], tmp));

		}
	}

	std::vector<int> subrootpath;
	rootpath.subpath (connec, subrootpath);
	rootpath.assign (subrootpath);

	for (auto && p : v) {
		rca::Path tmp(rootpath);		
		tmp.join (p.second); //remove nos duplicados
		p.second = tmp;
		newpaths.push_back (p.second);
	}


	return newpaths;
}

template <class Container, class SteinerRpr>
bool PathExchange<Container, SteinerRpr>::check_change (
		std::vector<std::vector<int>>& ccs,
		int c1, int c2, int source, 
		rca::Link & old,
		Paths& paths, 
		std::vector<int>& distancia) 
{

	auto res = std::find(ccs[0].begin(),ccs[0].end(),source);
	if (res != ccs[0].end()) {

		if (c2 != old.getX() && c2 != old.getY()) {

			Paths copy;

			int old_head, connec;
			if ( find (ccs[1], old.getX()) == find (ccs[1], c2)) {
				old_head = old.getX();
				connec = old.getY();
			} else {
				old_head = old.getY();			
				connec = old.getX();
			}

			copy = this->prepare_paths (paths,connec,old_head,c2);

			return this->test_pathsize (c1,c2,copy,distancia);
		}

		//c2 is the node not in the tree
		return this->test_pathsize (c1,c2,paths,distancia);
	} else {

		if (c1 != old.getX() && c1 != old.getY()) {

			Paths copy;

			int old_head, connec;

			if (find (ccs[1], old.getX()) == find (ccs[1], c1) ) {
				old_head = old.getX();
				connec = old.getY();				
			} else {
				old_head = old.getY();				
				connec = old.getX();
			}
			
			copy = this->prepare_paths (paths,connec,old_head,c1);

			return this->test_pathsize (c2, c1,copy, distancia);

		}


		//c1 is the node not in the tree
		return this->test_pathsize (c2, c1, 
			paths, distancia);					
	}
}


template<class Container, class SteinerRpr>
bool PathExchange<Container, SteinerRpr>::do_change (
	SteinerRpr& st, Container& container, 
	rca::Link& novo, rca::Link& old, 
	rca::Network& network, rca::Group& g)
{
	typedef typename rca::OperationType OperationType;

	int trequest = g.getTrequest ();
	int band = (int)network.getBand (novo.getX(), novo.getY());	

	container.update_inline (novo, 
		OperationType::IN, trequest, band);

	int cost = (int)network.getCost (novo.getX(), novo.getY());
	st.add_edge (novo.getX(), novo.getY(), cost);					

	Prune p;
	p.prunning (st);
	int rcost = 0;
	for (auto e = p.begin(); e != p.end(); e++) {
		
		int x = e->first;
		int y = e->second;

		rcost += (int)network.getCost (x, y);						
		rca::Link l (x, y, 0);						

		int band = (int)network.getBand (l.getX(), l.getY());
		container.update_inline (l, OperationType::OUT, 
			trequest, band);
	}

	st.set_cost (st.get_cost () - rcost);

	band = (int)network.getBand (old.getX(), old.getY());
	container.update_inline (old, OperationType::OUT,
		trequest, band);

	return true;

}

template<class Container, class SteinerRpr>
bool PathExchange<Container, SteinerRpr>::core_search (
	SteinerRpr & st, 
	Container & container, 
	rca::Network& network, 
	rca::Link& old, 
	rca::Group& g)

{

	//inicialization of some variables
	int trequest = g.getTrequest ();	
	int NODES = network.getNumberNodes ();
	int tree_cost = st.get_cost ();
	std::vector<int> members = g.getMembers ();
	int source = g.getSource ();

	std::vector<rca::Path> tree_paths;
	std::vector<int> distancia(NODES);
	this->paths_distance (st, tree_paths, distancia, 
		members, source);

	//removing edges: old
	st.remove_edge (old.getX (), old.getY());
	tree_cost -= (int)network.getCost (old.getX(),old.getY());
	st.set_cost (tree_cost);

	//getting the components after removing the edge 'old'
	std::vector<std::vector<int>> ccs = this->get_components (st);

	for (auto c1 : ccs[0]) {				
		//getting the neighbors
		std::vector<int> neighbor = network.get_neighboors (c1);

		for (auto c2 : ccs[1]) {
			if (network.getCost (c1, c2) > 0) {

				rca::Link novo(c1, c2, 0);
				int band = network.getBand (c1, c2);
				if (!tocontinue (novo, old, container, trequest, band)) {
					continue;
				}

				bool canchange = this->check_change (ccs, 
					c1, c2, source, old, tree_paths, distancia);
				
				if (canchange) {					

					//evita loop infinito com trocas repedidas
					if (this->control_trocas (old, novo)) {
						int cost = network.getCost (old.getX(), old.getY());
						st.add_edge (old.getX (), old.getY(), cost);
						return false;
					}

					//realiando mudança
					return this->do_change (st, container, 
						novo, old, network, g);

				}

			}
		}
	}

	int cost = network.getCost (old.getX(), old.getY());
	st.add_edge (old.getX (), old.getY(), cost);
	
	return false;

}

template <class Container, class SteinerRpr>
bool PathExchange<Container, SteinerRpr>::run (
	std::vector<SteinerRpr>& solucao, 
	Container& container, 
	rca::Network& network, std::vector<rca::Group>& mgroups) 
{

	int GROUPS = mgroups.size ();

	auto begin = container.m_heap.ordered_begin ();
	auto end = container.m_heap.ordered_end ();

	do {

		rca::Link l = *begin;
		bool res = false;
		for (int i = 0; i < GROUPS; ++i)
		{
			int x = begin->getX();
			int y = begin->getY();
			if (solucao[i].find_edge (x,y)) {
				res = this->core_search (solucao[i], container, 
					network, l, mgroups[i]);

				if (res) {
					begin = container.m_heap.ordered_begin ();
					end = container.m_heap.ordered_end ();
					return true;
				}
			}
		}
		begin++;
	} while (begin != end);

	return false;

}

template class rca::sttalgo::PathExchange<rca::EdgeContainer<rca::Comparator,rca::HCell>,steiner>;