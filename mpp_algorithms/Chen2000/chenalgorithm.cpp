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
 * Este método tem por objetivo substituir o Link link
 * da árvore st por outro link da árvore. 
 * 
 */
std::vector<int> Chen::cut_edge (STTree & st, rca::Link & link) {
	
	//union_find operations
	int NODES = m_net->getNumberNodes ();
	std::vector<int> nodes_mark = std::vector<int> (NODES,0);
	
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
		
		cout << "Processing X: " << x <<endl;
		cout << "Processing Y: " << y <<endl;
		
		for (auto it = edges.begin (); it != edges.end(); it++) {
			//se it->getX() é igula a x, então verifca se a outra
			//aresta foi processada
			if (!nodes_x.empty()) {
			 if ( (it->getX () == x) && (nodes_mark[it->getY ()] == 0) ) {
				//senão adicione-a a lisda de marcados e guarde 
				//para processamento
				nodes_mark[it->getY ()] = nodes_mark[x];
				nodes_x.push (it->getY ());
				
				//faz o mesmo para o caso da aresta se it->getY()
			 } else if ( (it->getY () == x) && (nodes_mark[it->getX ()] == 0) ) {
				nodes_mark[it->getX ()] = nodes_mark[x];
				nodes_x.push (it->getX ());
			 }
			}
			 
			 //processo semelhante para o nó y
			 if (!nodes_y.empty()) {
			 if (it->getX () == y && nodes_mark[it->getY ()] == 0) {
				nodes_mark[it->getY ()] = nodes_mark[y];
				nodes_y.push (it->getY ());
			 } else if (it->getY () == y && nodes_mark[it->getX ()] == 0) {
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

void Chen::run () {
	
	bool running = true;
	while (1) {
		
		std::vector<rca::Link> LE = sort_edges ();
		int Z = get_max_congestion ();
		
		for (auto it = LE.begin (); it != LE.end(); ++it) {
			
			//here the code for rebuild is made
			for (auto st_it = m_trees.begin (); 
				 st_it != m_trees.end(); st_it++) {
				
				STTree sttr = *st_it;
				if (std::find(sttr.edges.begin(),sttr.edges.end(), *it) != sttr.edges.end()) {
					cout << "Aresta: " << *it << " Está em " << st_it->id << endl;
					
					cut_edge (sttr,*it);
					
					getchar ();
				}
			}
			
		}
		
		running = false;
		
		if (!running) {
			break;
		}
	}
	
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