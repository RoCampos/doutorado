#include "steiner.h"

using std::cout;
using std::endl;

steiner::steiner () : m_edges(0), m_nodes(0), m_cost (0){

}

steiner::steiner (int numvertices) 
: m_edges(0), m_nodes(numvertices), m_cost (0) {

	this->m_adjacentList.resize (numvertices);

}

steiner::steiner (int numvertices, const std::vector<int> & terminals) 
: m_edges(0), m_nodes(numvertices), m_cost (0){

	std::vector<int> v(0);

	this->m_adjacentList.resize (numvertices);
	this->m_terminals = std::vector<int> (terminals);

}


steiner::steiner (int numvertices, int source, const std::vector<int> & terminals) 
: m_edges(0), m_nodes(numvertices), m_cost (0) {

	std::vector<int> v(0);

	this->m_adjacentList.resize (numvertices);
	this->m_terminals = std::vector<int> (terminals);
	this->m_terminals.push_back (source);

}

steiner::steiner (const steiner & ref)
: m_edges(ref.m_edges), m_nodes(ref.m_nodes), m_cost (ref.m_cost) {
	
	this->m_adjacentList = ref.m_adjacentList;
	this->m_terminals = ref.m_terminals;

}

steiner & steiner::operator= (const steiner & ref) {
	
	this->m_edges = ref.m_edges;
	this->m_cost = ref.m_cost;
	this->m_nodes = ref.m_nodes;
	this->m_adjacentList = ref.m_adjacentList;
	this->m_terminals = ref.m_terminals;

	return *this;
}

steiner::~steiner (){	
	
}

//takes O(E)
bool steiner::add_edge (int x, int y, double weight) {

	//se a aresta não existe adicione-a
	if ( !this->find_edge (x, y)) {

		this->m_adjacentList[x].push_back (y);
		this->m_adjacentList[y].push_back (x);

		this->m_cost += weight;
		this->m_edges++;

		return true;
	}

	return false;
}

//takes 2O(E)
bool steiner::remove_edge (int x, int y){

	bool hasRemoved = false;

	auto iter = std::find (
		this->m_adjacentList[x].begin(), 
		this->m_adjacentList[x].end(), y);

	if (iter != this->m_adjacentList[x].end()) {
		
		this->m_adjacentList[x].erase (iter);
		//atualiza o número de arestas
		hasRemoved = true;
	}

	iter = std::find (
		this->m_adjacentList[y].begin(), 
		this->m_adjacentList[y].end(), x);

	if (iter != this->m_adjacentList[y].end()) {
		
		this->m_adjacentList[y].erase (iter);

	}

	if (hasRemoved) {
		this->m_edges--;
		return true;
	}

	return false;
}

//takes O(E)
//retorna true se a aresta existe
bool steiner::find_edge (int x, int y) {

	if (this->m_adjacentList[x].size () < 
		this->m_adjacentList[y].size () ) {

		auto iter = std::find (
			this->m_adjacentList[x].begin(), 
			this->m_adjacentList[x].end(), 
			y);

		return iter != this->m_adjacentList[x].end();

	} else {
		auto iter = std::find (
			this->m_adjacentList[y].begin(), 
			this->m_adjacentList[y].end(), 
			x);

		return iter != this->m_adjacentList[y].end();
	}

}

const std::vector<int> & steiner::get_adjacent_vertices (int x) {
	return this->m_adjacentList[x];
}

int steiner::get_num_edges (){ 
	return this->m_edges;
}


int steiner::get_num_nodes () {
	return this->m_nodes;
}

void steiner::set_cost (double cost) {
	this->m_cost = cost;
}

double steiner::get_cost () {
	return this->m_cost;
}

int steiner::get_degree (int x) {
	return this->m_adjacentList[x].size ();
}

void steiner::set_terminals (const std::vector<int> & term) {
	this->m_terminals = std::vector<int> (term);
}

const std::vector<int> steiner::get_terminals(){
	return this->m_terminals;
}

bool steiner::is_terminal (int x) {
	auto iter = std::find (m_terminals.begin (), m_terminals.end(), x);
	return iter != m_terminals.end();
}

void steiner::print () {

	cout << "Graph{\n";

	for (auto m : this->m_terminals) {
		cout << m << "[color=red];\n";
	}

	auto ptr = this->get_all_edges ();
	for (std::pair<int,int> edge : ptr) {
		cout << edge.first << "--" << edge.second << endl;
	}
	cout << "}\n";

}

std::vector<std::pair<int,int>> steiner::get_all_edges () {

	int NODES = this->get_num_nodes ();

	std::vector<std::pair<int,int>> edges;

	std::vector<bool> marked (NODES, false);
	for (auto i : this->m_terminals) {
		get_edges (i, edges, marked);
	}

	return edges;
}

void steiner::prunning () {
	Prune prunner;
	prunner.prunning ( *this);
}

std::vector<int> steiner::get_path (int member, int source) {

	std::vector<int> path;

	std::vector<int> pred = std::vector<int> (this->m_nodes, 0);
	std::vector<int> mark = std::vector<int> (this->m_nodes, 0);

	std::queue<int> queue;
	queue.push (source);

	mark[source] = 1;
	pred[source] = -1;

	while (!queue.empty()) {

		int curr_node = queue.front ();

		if (curr_node == member) break;

		for (int next_node : this->m_adjacentList[curr_node]) {

			if ( mark[next_node] == 0 ) {

				queue.push (next_node);
				mark[next_node] = 2;

				pred[next_node] = curr_node;

			}

		}

		queue.pop ();

	}

	int next = member;
	while (next != -1) {
		path.push_back (next);
		next = pred[next];
	}

	return path;
}

// ----------------------------------- PRUNE CLASS --------------------------------- //

void Prune::prunning (steiner & tree) {

	//cout << "Prunning\n";

	bool hasNonLeaf = true;
	std::vector<int> members = tree.get_terminals ();

	int NODES = tree.get_num_nodes ();

	while (hasNonLeaf) {

		//cout << "Prune!\n";
		hasNonLeaf = false;

		for (int i=0;i < NODES; i++) {

			//cout << "Node: " << i << " degree(" << tree.get_degree (i) << ")"<< endl;

			if (tree.get_degree (i) == 1 && !tree.is_terminal (i)) {

				std::vector<int> adj = tree.get_adjacent_vertices (i);
				int j = adj[0];
				tree.remove_edge (i,j);

				std::pair<int,int> pair = std::make_pair(i,j);
				this->m_removed_edges.push_back (pair);
				hasNonLeaf = true;
				break;
			}
		}
	}

	// for (auto e : this->m_removed_edges) {
	// 	cout << e.first << " " << e.second << endl;
	// }

}