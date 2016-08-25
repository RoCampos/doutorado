#include <iostream>

#include "rca.h"

typedef rca::EdgeContainer<rca::Comparator, rca::HCell> Container;
typedef rca::sttalgo::SteinerTreeObserver<Container,steiner> Observer;
typedef rca::OperationType OperationType;

using std::cout;
using std::endl;

void stefan_version1 (
	rca::Network&, 
	std::vector<rca::Group>&, 
	size_t H);

bool feasible (std::vector<int> &in, rca::Path & path) {

	for (size_t i = 0; i < path.size ()-1; ++i)
	{
		if (in[ path[i] ] >= 1) {
			return false;
		}
	}
	return true;

}


//// ----------------- greedy approach -------
// 
// esta versão utiliza o algorithm de KShortestPath
// para criar um caminho viável da fonte para um nó 
// destino em questão.

void greedy_approach (
	rca::Network&, 
	std::vector<rca::Group>&, 
	size_t H);

void prunning (
	steiner&, 
	Container&,
	int tk, 
	rca::Network&);

/// --------------------- testes
void steiner_test (std::string file, int H) {

	rca::Network network;
	Group group;
	get_steiner_info (network, group, file);	
	std::vector<rca::Group> mgroups;	
	mgroups.push_back (group);
	greedy_approach (network, mgroups, H);

}

void multiple_test (std::string file, int H) {
	
	rca::Network network;
	std::vector<rca::Group> mgroups;
	get_problem_informations (file, network, mgroups);
	greedy_approach (network, mgroups, H);
}

void stefan_test (std::string file, int H) {
	rca::Network network;
	std::vector<rca::Group> mgroups;
	get_problem_informations (file, network, mgroups);
	stefan_version1 (network, mgroups, H);
}

int main(int argc, char const *argv[])
{

	srand (time(NULL));
	
	std::string file = argv[1];

	int H = atoi (argv[2]);
	//steiner_test (file, H);
	//multiple_test (file, H);
	stefan_test (file, H);
	

	return 0;
}

void greedy_approach (
	rca::Network& net,
	std::vector<rca::Group>& mgroups, size_t H) 
{

	auto func = rca::sttalgo::remove_top_edges<Container>;

	//mantando vetor de acesso aleatório
	int GROUP_SIZE = mgroups.size ();
	int NODES = net.getNumberNodes ();
	std::vector<int> vec = 
		rca::func::get_random_vector(GROUP_SIZE);

	Container container(NODES);
	Observer ob;
	ob.set_network (net);
	ob.set_container (container);

	int cos = 0;
	int res = 0;

	//loop guloso para construção de solução
	for (auto && i : vec)
	{
		int trequest = mgroups[i].getTrequest ();
		int w = mgroups[i].getSource ();
		std::vector<int> members = mgroups[i].getMembers ();

		steiner st = steiner(NODES, w, members);
		ob.set_steiner_tree (st, NODES);

		func (container, net, mgroups[i], 0);

		for(auto&& m : members) {
				
			KShortestPath ksp(&net);
			ksp.init (w,m);

			//busca um path com tamanho no máximo H
			//entre fonte w e um destino m.
			bool hpath = false;
			do {
				rca::Path path = ksp.next ();
				if (path.size ()-1 <= H) {
					
					cout << path << endl;

					for (size_t v = 0; v < path.size ()-1; v++){
						rca::Link l(path[v], path[v+1], 0);
						int cost = net.getCost (l.getX(), l.getY());
						int band = net.getBand (l.getX(), l.getY());
						ob.add_edge (l.getX(), l.getY(),cost, trequest, band);
					} 
					break;
				}
			} while (ksp.hasNext () && !hpath);
		}
		
		//making prunning
		prunning (ob.get_steiner_tree (), 
			container, trequest, net);

		//cout << ob.get_steiner_tree (). get_cost () << endl;
		cos += ob.get_steiner_tree (). get_cost ();
		// ob.get_steiner_tree ().print ();

	}
	res = container.top ();

	cout << cos << " " << res << endl;

}

void prunning (steiner& st,
	Container& container, 
	int trequest, 
	rca::Network& network)
{

	Prune p;
	p.prunning (st);
	int rcost = 0;
	for (auto e = p.begin(); e != p.end(); e++) {
		int x = e->first;
		int y = e->second;		
		rcost += (int)network.getCost (x, y);
		//atualizando capacidade residual
		rca::Link l (x, y, 0);						

		int band = network.getBand (l.getX(), l.getY());
		container.update_inline (l, OperationType::OUT, 
			trequest, band);

	}

}	

void stefan_version1 (rca::Network& network, 
	std::vector<rca::Group>& m_groups, 
	size_t H)
{

	std::vector<int> tree, term;

	//posição do vértice na lista
	std::vector<int> position (network.getNumberNodes());

	//entrada no vértice
	std::vector<int> invertex (network.getNumberNodes());

	int source = m_groups[0].getSource ();
	tree.push_back (source);
	term.push_back (source);
	position[source] = 0;

	std::vector<int> members = m_groups[0].getMembers ();

	while (term.size ()-1 < members.size ()) {

		rca::Path best; 
		for (auto s : tree) {

			int cost = std::numeric_limits<int>::max();

#ifdef DEBUG1
			cout << "s:" << s << endl;
#endif

			for (auto&& m : members) {	

				if (rca::func::in (tree, m)) continue;

				rca::Path path = shortest_path (s, m, network);
				
				bool t1 = (position[s] + path.size ()-1) <= H;
				bool t2 = feasible (invertex, path);

				if (t1 && t2) {

					int tmp_cost = 0;
					for (size_t i = 0; i < path.size()-1; ++i)
					{
						tmp_cost += network.getCost (path[i],path[i+1]);

					}

					if (tmp_cost < cost) {
						best = path;
						cost = tmp_cost;
					}

				}
#ifdef DEBUG1
				cout << "\t" << path << ": " << t1 << "," << t2 << "|" << cost << endl;
#endif

			}//fim for loop on members

			if (best.size () == 0) continue;
		} //fim for loop on tree

#ifdef DEBUG1
		cout << "<<<END\n";
		for (size_t i = 0; i < position.size ()-1; ++i)
		{
			cout << i << ": " << position[i] << " : ";
			cout << invertex [i] << endl;
		}
#endif

		if (best.size () > 0) {			

			//evitar loop
			for (size_t i = 0; i < best.size ()-1; ++i)
			{
				if (!rca::func::in (term, best[i])) {
					term.push_back (best[i]);
				}

				invertex[ best[i] ] += 1;
			}

		}

		cout << best << endl;

		for (size_t i = 0; i < best.size ()-1; i++ ) {
			if (!rca::func::in (tree, best[i])) {
				tree.push_back ( best[i] );
				int org = position[ best[best.size ()-1] ]; 

				//posição do vértex atualizado
				int j_pos = (best.size ()-1 - i);

				//u_i do elemento é igual ao valor de u_i de org + 
				//sua posição no caminho best
				position[ best[i] ] = j_pos + org;
			}

			//adiciona terminais no caminho
			if (rca::func::in (members, best[i])) {
				if (!rca::func::in (term, best[i])) {
					term.push_back (best[i]);
				}
			}

		}

	}


	// for (size_t i  = 0; i < position.size ()-1; ++i)
	// {
	// 	cout << i << ": " << position[i] << " : ";
	// 	cout << invertex[i] << endl;
	// }

}


