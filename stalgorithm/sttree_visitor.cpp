#include "sttree_visitor.h"

template<class Container>
void prunning (STTree & st, Container & cont, int trequest, int band)
{
	list_leafs_t& list = st.get_leafs (); 
	leaf_t * aux = list.begin;

	while ( aux != NULL) {
		
		leaf_t * tmp = aux->next;
		
		int id = aux->id;
		st.get_node(id).decrease_degree ();
		
		if (st.get_node(id).get_degree () == 0) {
			
			edge_t * edge = st.get_node(id).remove_adjacent_vertex ();
			
			int _other = (edge->x == id ? edge->y : edge->x);
			
			//removing leaf
			list.remove ( st.get_node(id).leaf );
			
			//printf ("Removing (%d - %d)\n",edge->x, edge->y);
			//remove aresta
			if (edge != NULL) {
				edge->in = false;
				double m_cost = st.getCost () - edge->value;
				st.setCost (m_cost);
				
				rca::Link l (edge->x, edge->y, -1);
				if (cont.is_used (l)){
					l.setValue ( cont.value (l));
				} else {
					std::cout << l <<":"<<l.getValue() << " not removed\n";
				}
				
				if (l.getValue () + 1 == band) {
					cont.erase (l);
				} else if (l.getValue() > -1){
					cont.erase (l);
					l.setValue ( l.getValue () + 1 );
					cont.push  (l);
				}
			}
			
			edge = NULL;
			
			//considera o outro vertex
			st.get_node(_other).decrease_degree ();
			if (st.get_node(_other).get_degree () == 1 && !st.get_node(_other).is_terminal ()) {
				
				leaf_t *leaf = new leaf_t ( _other );
				list.add (leaf);
				
				st.get_node(_other).add_leaf_node (leaf);
				
				aux = list.first ();
				
				continue;
			}
			
		}
		
		aux = tmp;
		tmp = NULL;
	}
	
	aux = NULL;
}

// std::vector<rca::Path> sttree_to_path (std::vector<STTree> & trees, 
// 									   std::vector<rca::Group> & groups)
// {
// 	std::vector<rca::Path> paths;
// 	
// 	int g = 0;
// 	for (auto tree : trees) {
// 	
// 		//getting the source
// 		int source = groups[g].getSource ();		
// 		//getting members
// 		std::vector<int> members = groups[g].getMembers ();
// 		
// 		
// 		g++;
// 	}
// 	
// 	
// 	return paths;	
// }

rca::Path stree_to_path (STTree & st, int source, int term)
{
// 	std::cout << term << std::endl;
// 	edge_t * e = st.get_edges ().begin;
// 	rca::Path p;
// 	p.push (term);
// 	std::vector<int> visited = std::vector<int>(30,0);
// 	visited[term] = 1;
// 		
// 	while (e != NULL) {
// 		if (e->x == term) {
// 			visited[e->y] = 1;
// 			p = stree_to_path (st,source, e->y, visited, p);
// 			
// 		} else if (e->y == term) {
// 			visited[e->x] = 1;
// 			p = stree_to_path (st,source, e->x, visited, p);
// 			
// 		}
// 		
// 		e = e->next;
// 	}
// 	
// 	std::cout << p << std::endl;
// 	return p;
	
	std::vector<int> stack;
	std::vector<int> visited = std::vector<int>(30,0);
	visited[source] = 1;
	
	stack.push_back (source);
	int curr_node = stack[0];
	while (!stack.empty ()) {
		
		bool terminal = false;
	
		//getting next
		edge_t *e = st.get_edges().begin;
		while (e != NULL) {
		
			int next = -1;
			if (e->x == curr_node && visited[e->y] == 0) {
				stack.push_back (e->y);
				next = e->y;
			} else if (e->y == curr_node && visited[e->x] == 0) {
				stack.push_back (e->x);
				next = e->x;
			}
			
			if (next != -1) {
			
				if (st.get_node (next).terminal) {
					
					//printing the path
					for (auto i : stack) {
						std::cout << i << " ";
					}
					std::cout << std::endl;
// 					visited[next] = 1;
					//stack.pop_back ();
					//next = stack[ stack.size () - 1];
					
					//terminal = true;
					//break;
				} 
				visited[next] = 1;
				
				curr_node = next;
				e = st.get_edges().begin;
				continue;
			} 	
			
			e = e->next;
		}
		
		stack.pop_back ();
		curr_node = stack[ stack.size () - 1];
		
	}
	rca::Path p;
	return p;
}

rca::Path stree_to_path (STTree & st, int source, int out, 
						 std::vector<int> & visited,
						 rca::Path & p)
{
	int in = out;
	std::cout << in << std::endl;
	
	if (out == source) {
		p.push (out);
		return p;
	} else {
		rca::Path pp;
		edge_t * e = st.get_edges ().begin;
		
		p.push (in);
		
		while (e != NULL) {
			if (e->x == in && visited[e->y] == 0) {
				visited[e->y] = 1;
				p = stree_to_path (st, source, e->y, visited, p);
				return p;
			} else if(e->y == in && visited[e->x] == 0){
				visited[e->x] = 1;
				p = stree_to_path (st, source, e->x, visited,p);
				return p;
			}			
			e = e->next;
		}
		
		return p;
		
	}	
}

template void prunning<rca::EdgeContainer<rca::Comparator, rca::HCell>>
		(STTree& st, 
		rca::EdgeContainer<rca::Comparator, rca::HCell>& cont, 
		int treq, 
		int band);
