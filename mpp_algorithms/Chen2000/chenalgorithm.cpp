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
	string str;

	int id = 0;
	while (getline (file_,str)) {
		
		if (str.empty ()) {
			//increment the id of st_trees during creation of 
			//the trees.
			id++;
		}else {
			
			istringstream tmp_file(str);
			tmp_file >> y;
			tmp_file >> str;
			tmp_file >> str;
			str = str.substr (0, str.length () - 2);
			x = atoi ( str.c_str () );
		
			//cout << --x << " " << --y << endl;
			m_edges[y][x] += 1;
			m_edges[x][y] += 1;
			
			
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