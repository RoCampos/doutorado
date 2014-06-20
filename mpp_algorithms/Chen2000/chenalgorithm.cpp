#include "chenalgorithm.h"

using namespace std;

Chen::Chen (std::string _instance, std::string _input, int alpha) : m_alpha(alpha) {
	
	m_input = _input;
	m_instance = _instance;
	
	m_net = make_shared<Network> ();
	init ();
	pre_processing ();
	m_congestion = get_max_congestion ();
	
	m_init_congestion = m_congestion;
}

void Chen::init () {
	
	Reader reader (m_instance);
	reader.configNetwork ( m_net.get () );
	m_groups = reader.readerGroup ();
	
}

void Chen::pre_processing () {
	
	int NODES = m_net->getNumberNodes ();
	m_edges = std::vector< std::vector<int>> (NODES);
	for (int i=0; i < NODES; i++) {
		m_edges[i] = std::vector<int> (NODES);
	}
	
	ifstream file_ (m_input.c_str ());
	
	int x;
	int y;
	string str_line;
	
	//creating st_tree
	int id = 0;
	m_trees.push_back (STTree(id));
	
	while (getline (file_,str_line)) {
		
		if (str_line.empty ()) {
			//increment the id of st_trees during creation of 
			//the trees.
			id++;
			m_trees.push_back (STTree(id));
		}else {
			
			//getting the edges from file
			istringstream tmp_file(str_line);
			std::string str;
			tmp_file >> y;
			tmp_file >> str;
			tmp_file >> str;
			str = str.substr (0, str.length () - 2);
			x = atoi ( str.c_str () );
		
			//updating the edges congestion
			//aqui subtrai um porque o GLPSOL roda com 
			//vertices de 1..n
			//na minha api os vértices são de 0..n-1
			--x;
			--y;
			m_edges[y][x] += 1;
			m_edges[x][y] += 1;
			
			//adding the edges to steiner trees.	
			m_trees[id].addEdge (x,y);
			
		}
		
	}
	
	file_.close ();
}

int Chen::get_max_congestion () {

	int NODES = m_net->getNumberNodes ();
	int max = INT_MIN;
	for (int i=0; i < NODES; i++) {
		for (int j =0; j < i;j++) {
			if (m_edges[i][j] > max) {
				max = m_edges[i][j];
			}
		}
	}
	return max;
}

std::vector<rca::Link> Chen::sort_edges () {
	
	std::vector<rca::Link> edges_congestion;
	
	int NODES = m_net->getNumberNodes ();
	for (int i=0; i < NODES; i++) {
		for (int j =0; j < i;j++) {
			//modificado (> 0) para (> 1)
			if (m_edges[i][j] > 0) {
				
				rca::Link link (i,j,m_edges[i][j]);
				edges_congestion.push_back(link);
			}
		}
	}
	
	std::sort (edges_congestion.begin(), 
			   edges_congestion.end(),
			   std::greater<rca::Link> () );
	
	return edges_congestion;
}

/**
 * Este método tem como objetivo fazer um corte na árvore
 * de Steiner st passada como parâmetro. Tal corte criará
 * duas subárvores. O corte é realizado removendo o link
 * passado como parâmetro.
 */
std::vector<int> Chen::cut_edge (STTree & st, rca::Link & link) {
	
	//union_find operations
	int NODES = m_net->getNumberNodes ();
	//marca as duas subárvores geradas
	std::vector<int> nodes_mark = std::vector<int> (NODES,-1);
	
	//estruturas auxiliares
	//filas usadas no processamento
	std::queue<int> nodes_x;
	std::queue<int> nodes_y;
	
	int x = link.getX();
	int y = link.getY();
	nodes_x.push ( x );
	nodes_y.push ( y );
	
	nodes_mark[x] = x;
	nodes_mark[y] = y;
	std::vector<rca::Link> edges = st.edges;
	while (!nodes_x.empty () || !nodes_y.empty() ) {
		
		if (!nodes_x.empty())
			x = nodes_x.front ();
		
		if (!nodes_y.empty())
			y = nodes_y.front ();
		
		for (auto it = edges.begin (); it != edges.end(); it++) {
			//se it->getX() é igula a x, então verifca se a outra
			//aresta foi processada
			if (!nodes_x.empty()) {
			 if ( (it->getX () == x) && (nodes_mark[it->getY ()] == -1) ) {
				//senão adicione-a a lisda de marcados e guarde 
				//para processamento
				nodes_mark[it->getY ()] = nodes_mark[x];
				nodes_x.push (it->getY ());
				
				//faz o mesmo para o caso da aresta se it->getY()
			 } else if ( (it->getY () == x) && (nodes_mark[it->getX ()] == -1) ) {
				nodes_mark[it->getX ()] = nodes_mark[x];
				nodes_x.push (it->getX ());
			 }
			}
			 
			 //processo semelhante para o nó y
			 if (!nodes_y.empty()) {
			 if (it->getX () == y && nodes_mark[it->getY ()] == -1) {
				nodes_mark[it->getY ()] = nodes_mark[y];
				nodes_y.push (it->getY ());
			 } else if (it->getY () == y && nodes_mark[it->getX ()] == -1) {
				nodes_mark[it->getX ()] = nodes_mark[y];
				nodes_y.push (it->getX ());
			 }
			 }
			 //getchar ();
		}
		
		//remove nó processado
		if (!nodes_x.empty())
			nodes_x.pop ();
		
		if (!nodes_y.empty())
			nodes_y.pop ();
	}
	return nodes_mark;
}

/**
 * Este método tem por objetivo substituir o Link link
 * da árvore st por outro link da árvore. 
 * 
 */
bool Chen::replace (STTree & st, rca::Link & link) {
	
	//guarda arestas que podem subustituir o Link link
	std::vector<rca::Link> newedges;
	//corte no grafo
	std::vector<int> cut_xy = this->cut_edge (st, link);
	//árvore com nó x = link.getX
	std::vector<int> Tx;
	//árvore com nó x = link.getY
	std::vector<int> Ty;
	
#ifdef DEBUG1
	cout << "--Debuggin cut_edge result--\n";
	
	for (int i=0; i < cut_xy.size (); i++) {
		cout << i<<":"<<cut_xy[i] << " ";
	}
	cout << endl;
	cout << "-------------------------\n";
#endif
	
	
	//separando árvore em árvore-x e árvore-y
	for (size_t i = 0; i < cut_xy.size (); i++) {
		if (cut_xy[i] == link.getX ()) {
			Tx.push_back (i);
		} else if (cut_xy[i] == link.getY ()) {
			Ty.push_back (i);
		}
	}
	
	//separando as arestas no corte entre x e y
	for (size_t i = 0; i < Tx.size (); i++) {
		for (size_t j = 0; j < Ty.size (); j++) {
			
			//testa se a aresta existe
			if ( m_net->getCost (Tx[i] , Ty[j]) > 0 ) {
				//verifica se não está congestioanda
				if (m_edges[ Tx[i] ][ Ty[j] ] < (get_max_congestion() -1) ) {
					//adiciona link para processamento posterior
					rca::Link l ( Tx[i], Ty[j], 0);
					
					if (l != link) {
						newedges.push_back ( l );
					}
				}
			}
		}
	}
	
	//printing the edges
#ifdef DEBUG1
	cout << "Arestas possíveis" << endl;
	for (size_t i = 0; i < newedges.size (); i++) {
		cout << newedges[i] << endl;
	}
#endif
	
#ifdef DEBUG1
	cout << "-------replacing--------" << endl;
	cout << "\tAntes\n";
	st.print ();
#endif
	if ( !newedges.empty()  ) {
		
		int xx = rand() % newedges.size();
		
		st.replace (link, newedges[xx]);
		int i = link.getX();
		int j = link.getY();
		m_edges[i][j] -= 1;
		m_edges[j][i] -= 1;
		
		i = newedges[xx].getX();
		j = newedges[xx].getY();
		m_edges[i][j] += 1;
		m_edges[j][i] += 1;
		
#ifdef DEBUG1
		cout << link <<"|"<<newedges[xx] << endl;
#endif
		
#ifdef DEBUG1
	cout << "\tdepois\n";
	st.print ();
#endif
		
		return true;
	} else {
		return false;
	}
	
}

void Chen::run () {
	
	bool running = true;
	
	int Z = get_max_congestion ();
	int cont = 0; 
	while (running) {
		
		std::vector<rca::Link> LE = sort_edges ();
		//-------------
#ifdef DEBUG1
		cout << "Arestas Congestionadas\n" << endl;
		for (auto it = LE.begin(); it != LE.end(); it++) {
			cout << (*it) << " : " << m_edges[it->getX()][it->getY()] << endl;
		}	
#endif
		//-------------
		for (auto it = LE.begin (); it != LE.end(); ++it) {
			
			//here the code for rebuild is made
			for (auto st_it = m_trees.begin (); 
				 st_it != m_trees.end(); st_it++) {
				
				STTree sttr = *st_it;
				if (std::find(sttr.edges.begin(),sttr.edges.end(), *it) != sttr.edges.end()) {
#ifdef DEBUG1
					cout << "Aresta: " << *it << " Está em " << st_it->id << endl;
#endif		
					running = replace (*st_it,*it);

#ifdef DEBUG1
cout << "Click enter to continue...\n";
getchar ();
#endif					
					
					if (running) {
						goto BREAK_TEST;
					}
#ifdef DEBUG11
cout << "Click enter to continue...\n";
					getchar ();
#endif		
				}
			}
		}
		
#ifdef DEBUG1
cout << "Click enter to continue and test BREAK_TEST...\n";
					getchar ();
#endif
		
		BREAK_TEST:
		if (!running) {
			break;
		} 
		
		cont++;
		if (cont % 50 == 0) {
			if (get_max_congestion() < Z) {
				Z = get_max_congestion ();
				cont=0;
			} else {
				running = false;
			}
		}
	}
	
#ifdef DEBUG1
	for (int i=0; i < m_edges.size (); i++) {
		for (int j = 0; j < m_edges[i].size (); j++) {
			cout << m_edges[i][j] << " "; 
		}
		cout << endl;
	}
#endif
#ifdef DEBUG
	for (int i=0; i < m_trees.size (); i++) {
		m_trees[i].print ();
		cout << endl;
	}
#endif

	
	Z = get_max_congestion ();
	cout << m_init_congestion << " ";
	cout << Z << endl;
}

void Chen::print_information () {
	
	cout << "Steiner Trees:" << m_trees.size () << endl;
	cout << "Initial Congestion: " << m_init_congestion << endl;
	
}

void Chen::print_edges_xdot () {
	int NODES = m_net->getNumberNodes ();
	cout << "Graph {\n";
	for (int i=0; i < NODES; i++) {
		for (int j =0; j < i;j++) {
			if (m_edges[i][j] > 0) {
				cout << i << "--" << j;
				cout << "[label=" << m_edges[i][j] << "]\n";
			}
		}
	}
	cout << "}\n";
}