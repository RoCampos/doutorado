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

std::vector<rca::Path> stree_to_path (STTree & st, int source, int nodes)
{
// 	st.xdotFormat ();
	std::vector<rca::Path> paths;
	
	std::vector<int> stack;
	std::vector<int> visited = std::vector<int>(nodes,0);
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
					rca::Path path;
					auto rit = stack.crbegin ();
					for (; rit != stack.crend(); rit++) {
						path.push (*rit);
					}
					paths.push_back (path);
					//std::cout << path << std::endl;

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
	
	return paths;
}

std::vector<int> make_cut_visitor (std::vector<rca::Link>& st, 
			   int source, 
			   rca::Link & link,
			   std::vector<int> & mark, 
			   int nodes)
{
	
#ifdef DEBUG
	printf ("BEGIN ---- %s ---- BEGIN\n",__FUNCTION__);
#endif 
	
	//all vertex of the cut reachable from source
	std::vector<int> tree_nodes;
	
	//used in the depth search first
	std::vector<int> stack;
	std::vector<int> visited = std::vector<int>(nodes,0);
	
	//mark source as visited
	visited[source] = 1;
	
	//init the stack
	stack.push_back (source);
	
	while (!stack.empty ()) {

		//getting the current node to process
		int curr_node = stack[ stack.size () - 1];
		
		//erasing the curr_node from stack
		stack.pop_back ();
		
		//mark curr_node as visited
		mark[curr_node] = source;
		
		//look for neighbor of curr_node
		for (rca::Link const& e : st){
			
			if (link == e) continue;
				
			//test if x ou y is a neighbor of curr_node
			//and add it to the stack
			if (e.getX() == curr_node && visited[e.getY()] == 0) {
				stack.push_back (e.getY());
				visited[e.getY()] = 1;
// 				break;
			} else if (e.getY() == curr_node && visited[e.getX()] == 0) {
				stack.push_back (e.getX());
				visited[e.getX()] = 1;
// 				break;
			}
		}
		
		//add curr_node to the vertex set
		tree_nodes.push_back (curr_node);		
	}
	
#ifdef DEBUG
	printf ("END ---- %s  ---- END\n",__FUNCTION__);
#endif 	
	
	return tree_nodes;
}

template void prunning<rca::EdgeContainer<rca::Comparator, rca::HCell>>
		(STTree& st, 
		rca::EdgeContainer<rca::Comparator, rca::HCell>& cont, 
		int treq, 
		int band);
