#include "svobheuristic.h"

using namespace rca;

typedef rca::EdgeContainer<rca::Comparator, rca::HCell> Container;
typedef rca::sttalgo::SteinerTreeObserver<Container, steiner> Observer;


rca::StefanHeuristic::StefanHeuristic (rca::Network& network, 
						std::vector<rca::Group>& groups) 
{
	this->m_network = &network;
	this->m_groups = groups;
}

void StefanHeuristic::run (size_t hoplimit) {


	this->H = hoplimit;

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

			std::vector<rca::Link> v = path_to_edges (path, m_network);
			for(auto&& link : v) {
				int band = m_network->getBand (link.getX(), link.getY());
				ob.add_edge (link.getX(), link.getY(), link.getValue (), trequest, band);			
			}
			
		} while (termT.size ()-1 != members.size ());
		
		m_solution.push_back(ob.get_steiner_tree ());
		m_solution[k].print ();
		

	} //end for over groups

	cout << "Z: ";
	cout << cg.top () << endl;

	cout << "Tree's cost\n";
	for(auto&& sol : m_solution) {
		cout << sol.get_cost () << endl;
	}

	cout << "AllCost: \t" << allcost << endl;

	cout << "\nChecking paths...\n";	
	// rca::sttalgo::check_path_limit (m_solution, m_groups, H);
	for (int i = 0; i < m_solution.size (); ++i)
	{
		cout << rca::sttalgo::check_path_limit (m_solution[i], m_groups[i], H) << endl;
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

