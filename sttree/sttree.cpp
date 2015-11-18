#include "sttree.h"

STTree::STTree (int nnodes, const std::vector<int> & terminals) {
	
	_init_nodes (nnodes);
	_init_terminals (terminals);
	
	m_cost = 0.0;
}

/*copyt constructor*/
STTree::STTree (const STTree & ref) {

	m_cost = 0.0;
	this->m_edges.clear (); //liberando arestas
	
	_init_nodes (ref.m_nodes.size ());
	for (const node_t & t: ref.m_nodes) {
		m_nodes [t.id].terminal = t.terminal;
	}
	
	edge_t * e = ref.m_edges.begin;
	while (e != NULL) {
		if (e->in)
			this->add_edge (e->x, e->y, e->value);
		e = e->next;
	}
	
	this->m_source = ref.m_source;
	
	//this->prunning ();
}

STTree & STTree::operator= (const STTree & ref) {
	
	if (&ref == this) {
		return *this;
	}
	
	this->m_edges.clear (); //liberando arestas
	m_cost = 0.0;
	_init_nodes (ref.m_nodes.size ());
	for (const node_t & t: ref.m_nodes) {
		//m_nodes [t.id].id = t.id;
		m_nodes [t.id].terminal = t.terminal;
	}
	
	edge_t * e = ref.m_edges.begin;
	while (e != NULL) {
		if (e->in)
			this->add_edge (e->x, e->y, e->value);
		e = e->next;
	}
	
	this->m_source = ref.m_source;
	
	//this->prunning ();
	
	return *this;
}


void STTree::add_edge (int x, int y, double value) {
	
	int w = x;
	x = (x>y? x : y);
	y = (y<x? y : w);
	
	edge_t * edge = new edge_t(x, y, value);
	
	m_cost += value;
	
	m_nodes[x].add_adjacent_vertex (edge);
	m_nodes[y].add_adjacent_vertex (edge);
	
	m_edges.add_edge (edge);
	
	edge->in = true;
	
	m_nodes[x].increase_degree ();
	m_nodes[y].increase_degree ();
	
	_leaf_traits (x);
	_leaf_traits (y);
	
	edge = NULL;
}

void STTree::_leaf_traits (int node) {

	if ( !m_nodes[ node ].is_terminal () ) {
		
		//se for grau dois added to list of leafs
		if ( m_nodes[ node ].get_degree() == 1) {
		
			leaf_t *leaf = new leaf_t ( node );
			m_leafs.add (leaf);
			m_nodes[ node ].add_leaf_node (leaf);
			
		} else if (m_nodes[ node ].get_degree() == 2) {
		
			m_leafs.remove ( m_nodes[ node ].leaf );
			
		}
		
	}
		
}

void STTree::prunning () {

#ifdef DEBUG1
	printf ("------------%s-------------\n", __FUNCTION__);
#endif
	
	leaf_t * aux = m_leafs.first ();

	while ( aux != NULL) {
		
		leaf_t * tmp = aux->next;
		
		int id = aux->id;
		m_nodes[id].decrease_degree ();
		
		if (m_nodes[ id ].get_degree () == 0) {
			
			edge_t * edge = m_nodes[ id ].remove_adjacent_vertex ();
			
			int _other = (edge->x == id ? edge->y : edge->x);
			
			//removing leaf
			m_leafs.remove ( m_nodes[ id ].leaf );
			
			//printf ("Removing (%d - %d)\n",edge->x, edge->y);
			//remove aresta
			if (edge != NULL) {
				edge->in = false;
				m_cost -= edge->value;
			}
			
			edge = NULL;
			
			//considera o outro vertex
			m_nodes[_other].decrease_degree ();
			if (m_nodes[_other].get_degree () == 1 && !m_nodes[_other].is_terminal ()) {
				
				leaf_t *leaf = new leaf_t ( _other );
				m_leafs.add (leaf);
				
				m_nodes[ _other ].add_leaf_node (leaf);
				
				aux = m_leafs.first ();
				
				continue;
			}
			
		}
		
		aux = tmp;
		tmp = NULL;
	}
	
	aux = NULL;
		
}

void STTree::xdot_format () {
	
	std::cout << "Graph {\n";
	for (unsigned int i = 0; i < m_nodes.size (); i++) {
		if (m_nodes[i].is_terminal() ) {
			//printf ("%d [format=circle,color=red];\n",i);
			std::cout << i << "[format=circle,color=red];\n";
		}
	}
	
	edge_t * aux = m_edges.begin;
	while (aux != NULL) {
	
		if (aux->in)
			printf ("%d -- %d;\n", aux->x, aux->y);
		else 
			printf ("%d -- %d[style=dashed];\n", aux->x, aux->y);
		
		aux = aux->next;
	}
	
	std::cout << "}\n";
	
}