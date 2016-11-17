#include "steiner_tree_factory.h"

using namespace rca::sttalgo;
using namespace rca;

template <class Container, class SteinerRepr>
void AGMZSteinerTree<Container, SteinerRepr>::create_list (rca::Network& network)
{

	this->m_links.clear ();

	auto iter = network.getLinks ().begin();
	auto end = network.getLinks ().end();
	for (; iter != end; iter++) {
		this->m_links.push_back (*iter);
	}
		
	for (auto & it: this->m_links) {
		it.setValue (0);
	}	
}

template <class Container, class SteinerRepr>
void AGMZSteinerTree<Container, SteinerRepr>::build (
	SteinerTreeObserver<Container, SteinerRepr> & sttree, 
	rca::Network & network, 
	rca::Group & g,
	Container& cg)
{

	
	std::vector<rca::Link> links = this->m_links;
		
	int pos = 0;
	
	int trequest = g.getTrequest ();

	//O(E)
	while (!links.empty()) {
		
		rca::Link link = links[pos];
						
		int cost = network.getCost (link.getX(), link.getY());
			
		if (network.isRemoved(link)){
			links.erase ( (links.begin () + pos) );
			continue;
		}
		
		int BAND = network.getBand (link.getX(), link.getY());

		// sttree.add_edge (link.getX(), link.getY(), cost, BAND);
		sttree.add_edge (link.getX(), link.getY(), cost, trequest, BAND);
		links.erase ( (links.begin () + pos) );
	}
	
}

template <class Container, class SteinerRepr>
void AGMZSteinerTree<Container, SteinerRepr>::update_usage (rca::Group& g,
						rca::Network & m_network,
						SteinerRepr & st)
{


	int trequest = g.getTrequest ();

	for (std::pair<int,int> e : st.get_all_edges()) {		
		rca::Link l (e.first, e.second, 0);
		int cost = m_network.getCost (l.getX(), l.getY());
		l.setValue (cost);

		auto link = std::find( this->m_links.begin (), this->m_links.end(), l);
		link->setValue ( link->getValue () + trequest);
		
	}

	
	std::sort(m_links.begin(), m_links.end());
	
}

/********* ------------ Shortest Path Class ------------ *********/

template <class Container, class SteinerRepr>
void ShortestPathSteinerTree<Container, SteinerRepr>::build (
	SteinerTreeObserver<Container, SteinerRepr> & sttree, 
	rca::Network & network, 
	rca::Group & g,
	Container& cg)
{
	
	// int source = g.getSource ();
	std::vector<int> members = g.getMembers ();
	members.push_back (g.getSource ());
	
	int pos = rand () % members.size ();
	int source = members[pos];
	members.erase (members.begin () + pos);

	std::vector<int> prev = all_shortest_path (source, members[0] , network);
		
	int trequest = g.getTrequest ();

	for (int m : members) {
		rca::Path path = get_shortest_path (source, m, network, prev);
		
		auto rit = path.rbegin ();
		for (; rit != path.rend()-1; rit++) {
	
			int x = *rit;
			int y = *(rit+1);
			
			rca::Link l(x, y, 0);
			
			int cost = network.getCost (l.getX(), l.getY());
			int BAND = network.getBand (l.getX(), l.getY());
			
			//sttree.add_edge (l.getX(), l.getY(), cost, BAND);			
			sttree.add_edge (l.getX(), l.getY(), cost, trequest, BAND);
		}			
	}
	
}

/** ------------------------ WildestSteinerTree ------------------- **/
template <class Container, class SteinerRepr>
void WildestSteinerTree<Container, SteinerRepr>::build (
	SteinerTreeObserver<Container, SteinerRepr> & sttree, 
	rca::Network & network, 
	rca::Group & g,
	Container& cg)
{

	int source = g.getSource ();
	std::vector<int> members = g.getMembers ();
	std::vector<int> prev = inefficient_widest_path (source, 
													members[0], 
													&network);
	int trequest = g.getTrequest ();
	for (int m : members) {
		rca::Path path = get_shortest_path (source, m, network, prev);
		
		auto rit = path.rbegin ();
		for (; rit != path.rend()-1; rit++) {
	
			int x = *rit;
			int y = *(rit+1);
			
			rca::Link l(x, y, 0);
			
			int cost = network.getCost (l.getX(), l.getY());
			int BAND = network.getBand (l.getX(), l.getY());
			
			// sttree.add_edge (l.getX(), l.getY(), cost, BAND);
			sttree.add_edge (l.getX(), l.getY(), cost, trequest, BAND);
		}			
	}
	
}

template <class Container, class SteinerRepr>
void WildestSteinerTree<Container, SteinerRepr>::update_band (rca::Group & g, 
												rca::Network& network,
												STTree & st)
{

	edge_t *e = st.get_edge ();
	while (e != NULL) {
		
		if (e->in) {
		
			int tk = g.getTrequest ();
			int band = network.getBand(e->x, e->y);
			network.setBand(e->x, e->y, band - tk);
			network.setBand(e->y, e->x, band - tk);
		}
		
		e = e->next;
	}
	
}

// ---------------------- INTERATIVE DEPTH FIRST SEARCH ------------------
template <class Container, class SteinerRepr>
void LimitedBreadthSearchFirst<Container, SteinerRepr>::build (
	SteinerTreeObserver<Container, SteinerRepr> & sttree, 
	rca::Network & network, 
	rca::Group & g,
	Container& cg)
{

	int NODES = network.getNumberNodes ();
	int trequest = g.getTrequest ();

	//getting all members nodes
	std::vector<int> members = g.getMembers ();
	//members.push_back (g.getSource ());
	
	//defining a 'source' of the search
	int pos = rand () % members.size ();
	//int source = members[pos];
	//members.erase (members.begin () + pos);
	int source = g.getSource ();

	std::vector<int> marked = std::vector<int> (NODES, 0);
	m_pred = std::vector<int> (NODES,0);

	std::vector<int> counter = std::vector<int> (NODES, 0);
	counter[source] = 0;

	//starting...	
	std::queue<int> queue;
	queue.push (source);

	m_pred[source] = -1;

	int term_count = 0;
	while (!queue.empty () || (term_count == members.size () - 1) ) {

		//get the first element of the queue
		int curr_node = queue.front ();

		marked[curr_node] = 1;

		//get the neighboors of the vertext 'curr_node'
		auto neighbors = network.get_neighboors (curr_node);

		//apply shuffle algorithm to get diversity
		std::random_shuffle (neighbors.begin (), neighbors.end ());


		//addind all neighboors of vertex 'curr_node' to be examined
		auto begin = neighbors.begin ();
		auto end = neighbors.end ();
		for ( ; begin != end; begin++) {

			int next_node = *begin;
			rca::Link l (curr_node, next_node, 0);

			//checking if the node has been visited and if the limit of the path has been found
			int limit = counter[next_node];		
			if (!network.isRemoved (l) && marked[next_node] == 0 && limit < this->LIMIT) {
				
				//add link to tree
				int cost = network.getCost (l.getX(), l.getY());
				int BAND = network.getBand (l.getX(), l.getY());
				sttree.add_edge (l.getX(), l.getY(), cost, trequest, BAND);

				//add node to queue.
				queue.push (next_node);
				marked[next_node] = 2;
				m_pred[next_node] = curr_node;
				counter[next_node] = counter[curr_node] + 1;			

				//updating terminal counting
				if (sttree.get_steiner_tree ().is_terminal (next_node) ) {
					term_count++;
				}

			}

		}

		queue.pop ();

	}

}

template <class Container, class SteinerRepr>
int LimitedBreadthSearchFirst<Container, SteinerRepr>::get_path_length (
	int member, int source)
{

	int next = member;
	int count = 0;
	do {

		next = this->m_pred[next];
		count++;
	} while (next != -1);

	return count;
}

// ------------------ PathLimitedLocaSearch ------------------- //

template <class Container, class SteinerRepr>
void PathLimitedSearchTree<Container, SteinerRepr>::build (
	SteinerTreeObserver<Container, SteinerRepr> & sttree, 
	rca::Network & network, 
	rca::Group & g,
	Container& cg)
{

	//list of path for each terminal member
	typedef std::vector<std::vector<rca::Path>> MPath;
	typedef SteinerTreeObserver<Container, SteinerRepr> Observer;
	typedef LimitedBreadthSearchFirst<Container, SteinerRepr> Factory;

	//a factory type: LimitedBreadthSearchFirst
	Factory * factory = new Factory (std::numeric_limits<int>::max());

	//observer used in the trees search
	Observer ob;
	ob.set_network (network);
	
	//getting the members of a group multicas
	std::vector<int> members = g.getMembers ();
	members.push_back (g.getSource ());

	//a 'matrix' of vertexs of size of members
	MPath m_paths = MPath ( (members.size ()-1) );

	int NODES = network.getNumberNodes ();	
	int trequest = g.getTrequest ();
	int real_source = g.getSource ();

	//for each member create a tree 'st' with this member
	//as source	
	for (int k = 0; k < members.size (); ++k)
	{
		
		Container cg;
		cg.init_congestion_matrix (NODES);
		cg.init_handle_matrix (NODES);

		ob.set_container (cg);

		//getting current member 'k'
		int curr_src = members[k];

		//getting the members of the group
		std::vector<int> _curr_members_ = members;

		//erasing 'curr_src' from members
		_curr_members_.erase (_curr_members_.begin() + k);

		//defining 'st' structure
		steiner st (NODES, curr_src, _curr_members_);

		//copying the group and changing the source
		rca::Group _g = g;
		_g.setSource (curr_src);

		ob.set_steiner_tree (st, NODES);

		//building the tree with 'curr_src' as source
		factory->build (ob, network, _g, cg);
		ob.prune (trequest, 0);

		//getting the paths of tree with 'curr_src' as source
		for (int i = 0; i < members.size () - 1; ++i)
		{
			
			int curr_member = members[i];
			rca::Path path = st.get_path (curr_member, real_source);

			// cout << path << ":" << path.size () - 1 << endl;
			if ( (path.size () - 1) <= this->m_limit && path.size () > 0 ) {
				m_paths[i].push_back (path);

			}
		}

	}

	//processing the paths to build the tree for group g
	this->build_result_tree (m_paths, sttree, network, g);

}

template <class Container, class SteinerRepr>
void PathLimitedSearchTree<Container, SteinerRepr>::build_result_tree (
	std::vector<std::vector<rca::Path>>& m_paths,
	rca::sttalgo::SteinerTreeObserver<Container,SteinerRepr>& observer, 
	rca::Network& network, rca::Group & g)
{


	int trequest = g.getTrequest ();
	std::vector<int> members = g.getMembers ();

	//for each member vertex
	for (size_t i = 0; i < members.size (); ++i)
	{

		//get the path with less cost and size within the limit
		int size = m_paths[i][0].size();
		rca::Path curr_path = m_paths[i][0];
		for (auto && path : m_paths[i]) {
			
			int psize = (path.size () - 1);
			if ( psize < size) {
				size = (path.size () - 1);
				curr_path = path;
			}

			if (psize == size && path.getCost () < curr_path.getCost())
			{
				size = (path.size () - 1);
				curr_path = path;	
			}
		}
		
		//adding the path to final tree
		auto begin = curr_path.rbegin ();
		auto end = curr_path.rend ()-1;
		for (; begin != end; begin++) {			
			rca::Link l (*begin, *(begin+1), 0);

			int cost = network.getCost (l.getX(), l.getY());
			int band = network.getBand (l.getX(), l.getY()); 
			l.setValue (cost);

			observer.add_edge (l.getX(), l.getY(), cost, trequest, band);
		}
	}
}

// --------------------------- minmax_factory ------------------------- //
template <class Container, class SteinerRepr>
void MinmaxSteinerFactory<Container, SteinerRepr>::build (
	SteinerTreeObserver<Container, SteinerRepr> & sttree, 
	rca::Network & network, 
	rca::Group & g,
	Container& cg)
{

	std::vector<int> srcs;
	srcs = g.getMembers ();
	srcs.push_back (g.getSource());	

	std::vector<int> bases;
	std::vector<int> costpath;
	std::vector<std::vector<int>> paths;

	assert (is_connected (network, g) == true);

	this->m_ptr_net->addPseudoEdges (srcs);
	for (auto & e : this->m_ptr_net->getLinks ()) {
		if (network.isRemoved (e)) {
			this->m_ptr_net->removeEdge (e);
		} else {
			this->m_ptr_net->undoRemoveEdge (e);
		}
	}


	//from algorithm
	voronoi_diagram (*this->m_ptr_net, bases, costpath, paths); 

	DataSMT * data =
		this->join_components (bases, costpath, paths, srcs);

	this->minimun_spanning_tree (data);

	this->rebuild_solution (data, paths, network);	

	//update_edges
	int trequest = g.getTrequest ();
	
	int solcost = 0;
	for(rca::Link e : data->links) {
		
		int band = (int)network.getBand (e.getX(), e.getY());
		int cost = (int)network.getCost (e.getX(), e.getY());
		bool res = sttree.add_edge (e.getX(), e.getY(), cost, trequest, band);	
		//se o link foi inserido na árvore, então atualiza container
		//e copia da rede mantida pelo factory
		if (res) {
			network.setBand (e.getX(), e.getY(), band-trequest);
			solcost += cost;
		}			
	}

	assert (sttree.get_steiner_tree().get_all_edges ().size () > 0);

	this->m_ptr_net->removePseudoEdges (srcs);

	delete data;
	data = NULL;

}


template <class Container, class SteinerRepr>
int MinmaxSteinerFactory<Container, SteinerRepr>::get_min_cap (
		rca::Network & net,
		rca::Path & p1, 
		rca::Path & p2, 
		rca::Link l
		)
{
	//get_bottleneck from algoritmo.h
	int bp1 = get_bottleneck (net, p1);
	int bp2 = get_bottleneck (net, p2);
	bp1 = std::min (bp1, bp2);
	bp2 =  (int)net.getBand(l.getX(), l.getY());
	bp1 = std::min (bp1,bp2);

	return bp1;	
}

template <class Container, class SteinerRepr>
DataSMT* MinmaxSteinerFactory<Container, SteinerRepr>::join_components (
	std::vector<int> & bases,
	std::vector<int> & costpath,
	std::vector<std::vector<int>> & paths,
	std::vector<int> & srcs
	)
{
	std::map<int, int> vertex, invertex;

	for (size_t i = 0; i < srcs.size (); ++i)
	{
		vertex[ srcs[i] ] = i;
		invertex[ i ] = srcs[i];
	}

	rca::Network * G = new Network (srcs.size (), 0);
	//links que formam a rede de distância
	std::vector<rca::Link> tree;	
	//usando para armazear a aresta que junta duas bases
	EdgeMap edgeMap;
	//all the data do convert to and form network distance
	DataSMT * data = new DataSMT;

	for(rca::Link edge : this->m_ptr_net->getLinks ()) {

		if (this->m_ptr_net->isRemoved(edge)) continue;

		int x = bases[edge.getX()];
		int y = bases[edge.getY()];

		//if x and y are in different bases
		if (x != y) {			

			int band = (int) this->m_ptr_net->getBand (edge.getX(), edge.getY());

			//link on network distance
			rca::Link v (vertex[x], vertex[y], band);

			//path that may connect the bases x and y
			rca::Path p1(paths[edge.getX()]);
			rca::Path p2(paths[edge.getY()]);

			//bottleneck of bases x and y (path between them)
			int dist = this->get_min_cap (*(this->m_ptr_net), p1, p2, edge);

			if (G->getBand(v.getX(), v.getY()) == 0) {

				int cost = costpath[edge.getX()];
				cost 	+= costpath[edge.getY()];
				cost 	+= this->m_ptr_net->getCost (edge);

				G->setCost (v.getX(), v.getY(), cost);
				G->setCost (v.getY(), v.getX(), cost);
				G->setBand (v.getY(), v.getX(), dist);
				G->setBand (v.getX(), v.getY(), dist);

				v.setValue (cost);
				tree.push_back (v);

				//edges can be recovered from vertex and invertex
				//based on index
				edgeMap[EdgePair(v.getX(), v.getY())] = edge;

			} else {

				if (dist > G->getBand(v.getX(), v.getY())) {

					G->setBand (v.getY(), v.getX(), dist);
					G->setBand (v.getX(), v.getY(), dist);

					edgeMap[EdgePair(v.getX(), v.getY())] = edge; 
				}				
			}
		}

	}

	data->G = G;
	data->invertex = invertex;
	data->vertex = vertex;
	data->edgeMap = edgeMap;
	data->links = tree;

	return data;

}

template <class Container, class SteinerRepr>
void MinmaxSteinerFactory<Container, SteinerRepr>::minimun_spanning_tree (
	DataSMT* data) 
{
	//tree resulting from data
	std::vector<rca::Link> result;
	DisjointSet2 ds (data->G->getNumberNodes ());

	//sorting the edges based on cost of the path(edge)
	std::sort (data->links.begin(), data->links.end());

	//bulding minimum spanning tree
	while ( !data->links.empty () ) {
		rca::Link l = data->links[0];
		if (ds.find2(l.getX()) != ds.find2(l.getY())) {
			ds.simpleUnion (l.getX(), l.getY());
			result.push_back (l);
		}
		auto it = data->links.begin();
		data->links.erase (it);
	}
	data->links.clear ();
	data->links = result;
}


template <class Container, class SteinerRepr>
void MinmaxSteinerFactory<Container, SteinerRepr>::rebuild_solution (
	DataSMT* data, 
	std::vector<std::vector<int>> & paths,
	rca::Network & network)
{
	std::vector<rca::Link> links;

	for(rca::Link e : data->links) {

		//getting base vertex
		int bx = e.getX(),  by = e.getY();

		//pair to get eges that connects bases bx and bxy
		std::pair<int,int> p(bx, by);

		//complete path connecting two bases
		rca::Link link = data->edgeMap[p];
		rca::Path p1 (paths[link.getX()]);
		rca::Path p2 (paths[link.getY()]);
		
		auto path = path_to_edges (p1, &network);
		auto it = links.end();
		
		if (path.size () > 0) {
			for (auto e : path) {
				auto it = std::find (links.begin(), links.end(), e);
				if (it == links.end()) {
					links.push_back (e);
				}					
			}
		}
		path = path_to_edges (p2, &network);
		auto end = links.begin();
		if (path.size () > 0) {
			for (auto e : path) {
				auto it = std::find (links.begin(), links.end(), e);
				if (it == links.end()) {
					links.push_back (e);
				}
			}
		}
		links.push_back (link);
	}
	data->links.clear ();
	data->links = links;

}


template class rca::sttalgo::SteinerTreeFactory<rca::EdgeContainer<rca::Comparator, rca::HCell>, STTree >; 
template class rca::sttalgo::AGMZSteinerTree<rca::EdgeContainer<rca::Comparator, rca::HCell>, STTree >;
template class rca::sttalgo::ShortestPathSteinerTree<rca::EdgeContainer<rca::Comparator, rca::HCell>, STTree >;
template class rca::sttalgo::WildestSteinerTree<rca::EdgeContainer<rca::Comparator, rca::HCell>, STTree >;

template class rca::sttalgo::ShortestPathSteinerTree<rca::EdgeContainer<rca::Comparator, rca::HCell>, steiner >;
template class rca::sttalgo::AGMZSteinerTree<rca::EdgeContainer<rca::Comparator, rca::HCell>, steiner >;
template class rca::sttalgo::SteinerTreeFactory<rca::EdgeContainer<rca::Comparator, rca::HCell>, steiner >; 
template class rca::sttalgo::LimitedBreadthSearchFirst<rca::EdgeContainer<rca::Comparator, rca::HCell>, steiner>;
template class rca::sttalgo::PathLimitedSearchTree<rca::EdgeContainer<rca::Comparator, rca::HCell>, steiner>;
template class rca::sttalgo::MinmaxSteinerFactory<rca::EdgeContainer<rca::Comparator, rca::HCell>, steiner >;