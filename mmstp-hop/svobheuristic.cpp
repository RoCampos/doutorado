#include "svobheuristic.h"

using namespace rca;

typedef rca::EdgeContainer<rca::Comparator, rca::HCell> Container;
typedef rca::sttalgo::SteinerTreeObserver<Container, steiner> Observer;


rca::StefanHeuristic::StefanHeuristic (rca::Network& network, 
						std::vector<rca::Group>& groups) 
{
	this->m_network = &network;
	this->m_groups = groups;
	this->m_verbose = false;
}

rca::StefanHeuristic::StefanHeuristic(rca::Network& network, 
	std::vector<rca::Group>& groups, bool verbose)
{
	this->m_network = &network;
	this->m_groups = groups;
	this->m_verbose = verbose;
}

void StefanHeuristic::run (size_t hoplimit) {


	this->H = hoplimit;
	this->m_version = 0;

	int NODES = m_network->getNumberNodes ();
	int GROUP = m_groups.size ();

	int allcost = 0;

	//CONTAINER to store the edge usage
	Container cg(NODES);

	//observer to handle changes on edges
	Observer ob;
	ob.set_container (cg);

	//for each multicas group, build a hop-limited
	//steiner tree
	for (int k = 0; k < GROUP; ++k)
	{

		int curr_source = this->m_groups[k].getSource ();
		std::vector<int> members = this->m_groups[k].getMembers ();
		int trequest = this->m_groups[k].getTrequest ();

		steiner st (NODES, curr_source, members);
		ob.set_steiner_tree (st, NODES);

		//vetree mantém os nós da árvore
		//m_terminals os nós terminais ainda não adicionads 
		//a arvore
		std::vector<int> verticesT, termT;

		//controlar a posição dos vértices nos caminhos
		std::vector<int> position = std::vector<int> (NODES);

		//controlar  a entrada nos vértices
		//grafos não-orietados, evita loops
		std::vector<int> invertex = std::vector<int> (NODES);

		verticesT.push_back (curr_source);
		termT.push_back (curr_source);
		position[curr_source] = 0;

		//enquanto os terminais não estão na árvore, run
		do {

			rca::Path path = 
				this->getShortestPath (position, invertex, verticesT, members);

			if (path.size () == 0) break;

			//updating invertex and position
			this->update_invertex (path, invertex, termT, members);

			//passa como parâmetro:
			//vertices de T(vertexT), os membros do grupo(members), 
			//vértices das posições (position)
			//nós terminais já inclusos(termT), e o caminho (path)
			this->update_position (verticesT, members, position, termT, path);

			allcost += path.getCost ();
			// cout << path << ":" << path.getCost () << endl;

			//adding the paths to steiner tree structure
			std::vector<rca::Link> v = path_to_edges (path, m_network);
			for(auto&& link : v) {
				int band = m_network->getBand (link.getX(), link.getY());
				ob.add_edge (link.getX(), link.getY(), link.getValue (), trequest, band);			
			}
			
		} while (termT.size ()-1 != members.size ());
		
		m_solution.push_back(ob.get_steiner_tree ());
		

	} //end for over groups

	cout << "Z: ";
	cout << cg.top () << " ";
	cout << "Cost: " << allcost << endl;

	if (this->m_verbose) {
		cout << "Tree's cost\n";
		for(auto&& sol : m_solution) {
			cout << sol.get_cost () << endl;
		}

		cout << "\nChecking paths...\n";	
		// rca::sttalgo::check_path_limit (m_solution, m_groups, H);
		for (size_t i = 0; i < m_solution.size (); ++i)
		{
			cout << rca::sttalgo::check_path_limit (m_solution[i], m_groups[i], H) << endl;
		}

		for(auto&& tree : this->m_solution) {
			tree.print ();
		}	
	}	

}

void StefanHeuristic::run2 (size_t hoplimit) 
{
	
	int NODES = m_network->getNumberNodes ();
	int GROUP = m_groups.size ();

	this->H = hoplimit;
	this->m_version = 1;
	this->m_solution = std::vector<steiner>(GROUP);

	int allcost = 0;

	//CONTAINER to store the edge usage
	Container cg(NODES);

	//observer to handle changes on edges
	Observer ob;
	ob.set_container (cg);

	for (int k = 0; k < GROUP; ++k)
	{

		int curr_source = this->m_groups[k].getSource ();
		std::vector<int> members = this->m_groups[k].getMembers ();		

		steiner st (NODES, curr_source, members);
		ob.set_steiner_tree (st, NODES);

		//vetree mantém os nós da árvore
		//m_terminals os nós terminais ainda não adicionads 
		//a arvore
		std::vector<int> verticesT, termT;

		//controlar a posição dos vértices nos caminhos
		std::vector<int> position = std::vector<int> (NODES);

		//controlar  a entrada nos vértices
		//grafos não-orietados, evita loops
		std::vector<int> invertex = std::vector<int> (NODES);

		verticesT.push_back (curr_source);
		termT.push_back (curr_source);
		position[curr_source] = 0;

		std::vector<rca::Path> paths = std::vector<rca::Path> (NODES);

		do {

			rca::Path path = this->getShortestPath (position, invertex, verticesT, members);

			if (path.size () == 0) break;

			this->update_invertex (path, invertex, termT, members);

			this->update_position_after (paths, verticesT, members, position, termT, path);			

			
		} while (termT.size ()-1 != members.size ());

		//processing the tree
		std::vector<rca::Path> treepaths;

		for(auto&& path : paths) {
			
			auto it = std::find (treepaths.begin(), treepaths.end(), path);
			if (it == treepaths.end() && path.size () > 0) {
				treepaths.push_back (path);
			}			
		}

		this->step4 (treepaths, position, members);		
		
		this->calculate_sol (treepaths, k, ob);
		allcost += this->m_solution[k].get_cost ();

	}//end of for over groups

	cout << "Z: ";
	cout << cg.top () << " ";
	cout << "Cost: " << allcost << endl;

	if (this->m_verbose) {
		cout << "Tree's cost\n";
		for(auto&& sol : m_solution) {
			cout << sol.get_cost () << endl;
		}

		cout << "\nChecking paths...\n";	
		// rca::sttalgo::check_path_limit (m_solution, m_groups, H);
		for (size_t i = 0; i < m_solution.size (); ++i)
		{
			cout << rca::sttalgo::check_path_limit (m_solution[i], m_groups[i], H) << endl;
		}	

		for(auto&& tree : this->m_solution) {
			tree.print ();
		}

	}



}

rca::Path StefanHeuristic::getShortestPath (
	std::vector<int> & position,
	std::vector<int>& invertex,
	std::vector<int>& verticesT,
	std::vector<int>& members) 
{
	
	rca::Path best;
	for (auto s : verticesT) {

		int cost = std::numeric_limits<int>::max();

		for (auto m : members) {
			
			//verifica se m já está na árvore		
			if (findin (verticesT, m)) continue;

			rca::Path tmp_path = shortest_path (s, m, *this->m_network);
			
			//check for feasible
			bool t1 = (position[s] + tmp_path.size ()-1) <= this->H;
			bool t2 = this->avoidloop (invertex, tmp_path);

			//version 1 is actavte, the algorithm
			//don't control the input degree of vertex
			if (this->m_version) {
				t2 = 1;
			}

			if (t1 && t2) {
				int curr_cost = this->get_cost (tmp_path);
				if (curr_cost < cost) {
					cost = curr_cost;
					best = tmp_path;
					best.setCost (cost);
				}
			}
		} //end of for on members
	} //end of for on components of the tree

	return best;
}

void StefanHeuristic::update_position (std::vector<int> & verticesT, 
		std::vector<int> & members, 
		std::vector<int> & position, 
		std::vector<int> & termT,
		rca::Path& path)
{

	for (size_t i = 0; i < path.size ()-1; i++ ) {

		int curr_node = path[i];

		if (!this->findin (verticesT, curr_node)) {
			
			verticesT.push_back ( curr_node );

			int org = position[ path[ path.size ()-1] ]; 

			//posição do vértex atualizado
			int j_pos = (path.size ()-1 - i);

			//u_i do elemento é igual ao valor de u_i de org + 
			//sua posição no caminho best
			position[ curr_node ] = j_pos + org;
		}

		//adiciona terminais no caminho
		if (this->findin (members, curr_node)) {
			if (!this->findin (termT, curr_node)) {
				termT.push_back (curr_node);
			}
		}

	}

}

void StefanHeuristic::update_position_after (
		std::vector<rca::Path> & paths,
		std::vector<int> & verticesT,
		std::vector<int> & members,
		std::vector<int> & position,
		std::vector<int> & termT,
		rca::Path& path)
{
	
	//position of the source of path
	int source_pos = position [path[path.size ()-1]];
	
	rca::Path rootpath = this->get_source_path (paths, path);

	for (size_t i = 0; i < path.size ()-1; ++i)
	{

		int curr_node = path[i];

		//get position of current node in the current path
		int curr_node_pos = (path.size () - i + source_pos) - 1;

		if (position[curr_node] == 0) {
			
			position[curr_node] = curr_node_pos;			
			// paths[curr_node] = path;			
			paths[curr_node] = rootpath;

		} else {

			if (position[curr_node] > curr_node_pos) {

				//update_position && replace_path
				position[curr_node] = curr_node_pos;

				// paths[curr_node] = path;
				paths[curr_node] = rootpath;

			} 

		}
		

		if (findin(members, curr_node) && !findin(termT, curr_node))	{
			termT.push_back (curr_node);
		}

		if (!findin (verticesT, curr_node)) {
			verticesT.push_back	 (curr_node);
		}

	}


}


// the method just get the path from root to the node 
// the is the root of the current 'path' added to tree.

// 	for example consider the following situation:
// 		a path 5-->1-->23 is in the tree T
// 		so a new node is added to tree by 1, 1-->14
//	 	so the path which connects 14 to tree T is: 5-->1-->14
// 		this the rootpath of 14 and will be returned
rca::Path StefanHeuristic::get_source_path (
		std::vector<rca::Path> & paths,
		rca::Path& path)
{

	//root of the corrent path
	int croot = path[path.size ()-1];

	//size of the path from croot was added to Tree T
	int prootsize = paths[croot].size ();

	//the root of the tree T
	int troot = paths[croot].size () > 0 ? paths[croot][prootsize-1] : -1;

	//getting the path from root of root of the tree T until root of
	//path 'croot'
	std::vector<int> rpath;
	troot != -1 ? paths[croot].subpath (croot, rpath) : 0;

	rca::Path rootpath (rpath);
	for (int i = path.size ()-1; i >=0; i--) {
		rootpath.push (path[i]);	
	}
	rootpath.reverse ();
	if (rootpath.size () == 0) {
		rootpath = path;
	}

	int cost = 0;
	for (size_t i = 0; i < rootpath.size ()-1; ++i)
	{		
		cost += this->m_network->getCost (rootpath[i],rootpath[i+1]);
	}
	rootpath.setCost (cost);

	return rootpath;

}


void StefanHeuristic::step4 (
	std::vector<rca::Path>& tpaths, 
	VectorI& position, VectorI & members)
{

#ifdef DEBUG1
	cout << "____" << __FUNCTION__ << "____" << endl;
	for(auto&& a : tpaths) {
		cout << a << endl;
	}
#endif

	// TODO the paths on tpaths are incorrect, maybe after join their parts
	// TODO here in the path sepearation step, take care of the order of vertex
	// to avoid put the parts of a path in a reverse order

	std::vector<rca::Path> outpaths;

	std::vector<int> terminals;

	for(auto&& m : members) {

		std::vector<rca::Path> currpaths;

		for(auto&& p : tpaths) {
			if (p.find_in (m)) {
				currpaths.push_back (p);
			}
		}

		if (currpaths.size () > 1) {

			int dist = std::numeric_limits<int>::max();
			rca::Path tmp;
			for(auto&& p : currpaths) {
				if (p.getRevPosition (m) < dist) {
					dist = p.getRevPosition (m);
					tmp = p;
				}
			}
			
			outpaths.push_back (tmp);

		} else {

			int cut = -1;
			for (size_t i = 1; i < currpaths[0].size ()-1; ++i)
			{
				int v = currpaths[0][i];

				int vdist = position[v] + i;

				if (vdist < position[m]) {
					cut = v;
					break;
				}

			}

			if (cut == -1) {
				outpaths.push_back (currpaths[0]);
			} else {
				int dist = std::numeric_limits<int>::max();
				rca::Path tmp;
				
				for(auto&& p : tpaths) {

					if (p.find_in (cut)) {
						if (p.getRevPosition (cut) < dist) {
							dist = p.getRevPosition (cut);
							tmp = p;
						}	
					}
					
				}

				std::vector<int> vvv;
				tmp.subpath (cut,vvv);
				vvv.push_back (cut);
				std::vector<int> vvvv;
				currpaths[0].revsubpath (cut, vvvv);
				
				vvv.insert (vvv.end(), vvvv.begin(), vvvv.end());
				rca::Path v1(vvv), v2(vvvv);
				v1.reverse ();

				outpaths.push_back (v1);
			}

		}


	}
	
	tpaths = outpaths;

}


void StefanHeuristic::calculate_sol (
		std::vector<rca::Path>& tpaths,
		int group, Observer& ob)
{

	int trequest = this->m_groups[group].getTrequest ();

	for(auto&& path : tpaths) {
			
		for (size_t i=0; i < path.size ()-1; i++) {
			int cost = this->m_network->getCost (path[i], path[i+1]);
			int band = this->m_network->getBand (path[i], path[i+1]);
			ob.add_edge (path[i],path[i+1], cost, trequest, band);
		}

	}

	this->m_solution[group] = ob.get_steiner_tree ();
	
}