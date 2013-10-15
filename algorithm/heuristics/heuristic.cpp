#include "heuristic.h"

rca::Network * m_network = NULL;
rca::Group * m_terminals = NULL;
std::vector<SteinerNode> _nodes;
std::vector<double> objs;
std::vector< std::vector< PathList > > m_host;
std::map<int,int> m_term_index;

/**
* Esta função é utilizada para inicializar as variáveis 
* utilizadas no algoritmo.
* 
* Variáveis auxiliares são inicializadas:
* 	- _nodes(lista de steiner nodes) = variável utilizada para cópia de vector em tempo constante
* 	- m_term_index = mapeia nó terminal real para indice. Facilita na recuperação e persitência dos paths.
*/
void init (rca::Network * net, rca::Group * term) {
	
	m_network = net;
	m_terminals = term;

	//mapping terminals to index	
	for (int i=0; i < m_terminals->getSize (); i++) {		
		m_term_index[ m_terminals->getMember (i)-1 ] = i;
	}

	int NODES = m_network->getNumberNodes ();
	_nodes = std::vector<SteinerNode>(NODES, SteinerNode(0,0,false));
	objs = std::vector<double>(1);
		
	for (int i = 0; i < NODES;i++) {
		_nodes[i].setIndex (i);
	}

	//informations about terminals
	for (int i = 0; i < m_terminals->getSize (); i++) {
		//decrementa o valor do terminal
		//Os vértices são representados de 0 até N-1
		int j = m_terminals->getMember (i) - 1;
		_nodes[j].setTerminal (true);
	}
}

/**
 * Método para iniciar a população.
 * Consiste em adicionar os nós de steiner a árvore de steiner.
 * Definir a quantidade de objetivos.
 * 
 */
void initPopulation (int size, std::vector<SteinerTree> & population) 
{

	for (int i=0; i < size; i++) {
		SteinerTree st ( 1 , m_network->getNumberNodes() );
		st.setTempStructures (objs,_nodes);
		population.push_back (st);
	}	

}

/**
 * 
 * Método recebe um ponteito para um objeto to tipo SteinerTree
 * e inicializa os atributos _nodes(lista de nós) e o número
 * de objetivos.
 * 
 */
void initSolution (SteinerTree * st) 
{
	//teoricamente é feito em tempo constante O(1)
	st->setTempStructures (objs,_nodes);

}

/**
 * Método utilizado para criar um árvore de Steiner.
 * Uma variáção do algoritmo de takahashi e Matsuyama.
 * 
 */
void createSolution (SteinerTree * st) {

	//quantidade de nós
	int TERM = m_terminals->getSize ();
	
	//terminal nodes
	std::vector<int> term = m_terminals->getMembers ();
		
	//conjuntos disjuntos para evitar laços
	DisjointSet2 disSet ( m_network->getNumberNodes () );
	
	//first node added to tree
	int v = rand () % TERM;
	//atualizando lista de terminais para remover v
	//escolhe primeiro nó e remove ele da lista
	auto it = term.begin () + v;
	//decremento necessário, pois graph usa 
	//valores de 0 até N-1.
	v = *it - 1;
	term.erase (it);
	
	while (term.size() > 0) {
	
		int w = rand () % term.size ();
		auto it = term.begin () + w;
		//decremento para trabalhar com valores no graph
		w = *it - 1;
		
		//rca::Path path = shortest_path (v,w,m_network);
		int _v = m_term_index[v];
		int _w = m_term_index[w];
		rca::Path path = m_host[_v][_w][0];
		for (unsigned int i=0; i < path.size () -1; i++) {
			int v = path[i];
			int w = path[i+1];
			
			if (disSet.find (v) != disSet.find (w)) {
				disSet.simpleUnion (v,w);
				st->addEdge (v, w, m_network->getCost (v,w));	
			}
		}
				
		term.erase (it);
		
		v = w;

	}
	
	st->prunning ();

}

/**
 * Método utilizado para criar um árvore de Steiner a partir
 * de uma lista de aresta.
 * 
 */
void createSolution (SteinerTree & st, std::vector<rca::Link>& edges)
{
	DisjointSet2 disset( m_network->getNumberNodes() );

	for (unsigned int k=0; k < edges.size (); k++) {
		
		rca::Link link = edges[k];
		
		if (disset.find (link.getX()) != disset.find (link.getY())) {	
			disset.simpleUnion (link.getX (), link.getY());
			
			st.addEdge (link.getX(), link.getY (), (int)link.getValue () );
		}
	}
		
	st.prunning ();
}

/**
 * Método utilizado para retornar todas as arestas de um indivíduo.
 * 
 * 
 */
void getEdgesFromIndividual (SteinerTree & st, std::vector<rca::Link>& edges)
{

	Edge *begin = st.listEdge.first ();		
	//adicionando arestas da árvore de Steiner
	while (begin != NULL) {
			
		int v = begin->i;
		int w = begin->j;
		double cost = m_network->getCost (v,w);
		rca::Link link (v,w,cost);
		edges.push_back (link);
			
		begin = begin->next;
	}	
}

/**
 * Obtém arestas do caminho path entre dois terminais t1, t2.
 * As arestas são adicionadas a lista edges.
 * 
 */
void getEdgesFromPaths (int t1, int t2, int path, 
						std::vector<rca::Link>& edges)
{

	//adiciona novas aresatas do caminho selecionado
	int SIZE = m_host[t1][t2][path].size ();
	for (int j = 0; j < SIZE -2; j++) {
			
		int v = m_host[t1][t2][path].at(j);
		int w = m_host[t1][t2][path].at(j + 1);
		rca::Link link (v, w, m_network->getCost(v, w) );
			
		edges.push_back (link);
	}

}

void getPathsFromSet (std::set<rca::Path>& paths, std::vector<rca::Link>& edges)
{

	auto it = paths.begin ();
	for (; it != paths.end (); it++) {

		for (unsigned int i=0; i < (*it).size () -1; i++) {
			int v = (*it).at(i);
			int w = (*it).at(i+1);

			rca::Link link(v,w, m_network->getCost (v,w) );
				
			edges.push_back (link);
		}				
	}

}

void initHostInfo (int K = 10) {

	int TERM = m_terminals->getSize();
	
	m_host = std::vector < std::vector< PathList > > (TERM, std::vector< PathList >(TERM) );

	KShortestPath kpaths(m_network);
	for (int i = 0; i < TERM; ++i ) {
		
		for (int j=0; j < i; j++) {
			
			if (i != j) {
				
				int v = m_terminals->getMember(i) - 1;
				int w = m_terminals->getMember(j) - 1;

				int count = K;
				
				kpaths.init ( v, w );
				while ( kpaths.hasNext () && (--count >= 0) ) {
					
					rca::Path path = kpaths.next ();
					
					m_host[i][j].push_back ( path );
					m_host[j][i].push_back ( path );
					
				}

				kpaths.clear ();
				
			}
		}		
	}
}

/**
 * Este método computa os path_size caminhos entre
 * dois terminais.
 * 
 */
void addHostInfo (int t1, int t2, int path_size) {
	
	//exit the program with the test is true
	if ( (t1 == t2) || (t1 < 0 || t2 < 0) || (path_size <= 0) ) {
		cout <<"Erro em: " << "heuristic.h" << " Linha: " << __LINE__ << endl;
		exit (1);
	}
	
	if (m_host.size () == 0) {
		int TERM = m_terminals->getSize();
		m_host = std::vector < std::vector< PathList > > (TERM, std::vector< PathList >(TERM) );
	}
	
	//getting the terminals
	/**
	 * Os parâetros t1 e t2 são os valores dos nós reais.
	 * t1-1 é necesário pois os nós são representados de 0 até n-1.
	 * Os indices i e j são as associações de t1 e t2 na matriz 
	 * de terminais.
	 */
	int i = m_term_index[t1-1];
	int j = m_term_index[t2-1];
	
	if (m_host[i][j].size () > 0) {
		return; //não necessidade de criar caminhos já existem
	}
	
	KShortestPath kpaths(m_network);
		
	kpaths.init ( t1-1, t2-1 );
	int count = path_size;
		 
	while (kpaths.hasNext () && --count >= 0) {
		
		rca::Path path = kpaths.next ();
		m_host[i][j].push_back (path);
		m_host[j][i].push_back (path);
	}
	
}


void write_path (std::string file) {

	std::ofstream _file (file.c_str() , std::ofstream::out );
	for (int i=0; i < m_terminals->getSize (); i++) {
		for (int j=0; j < m_terminals->getSize (); j++) {

			if (i != j) {
				_file << "BEGIN ( " << m_host[i][j].size() << " )\n";
				_file << "From: " << i << " (" << m_terminals->getMember (i) << ")";
				_file << " To: " << j << " (" << m_terminals->getMember (j) << ")\n";
				for (unsigned int k = 0; k < m_host[i][j].size (); k++) {
					_file << m_host[i][j][k] << " " << m_host[i][j][k].getCost () << endl;
				}
				_file << "END\n";
			}
		}			
		_file << endl;
	}
	_file.close ();

}

void read_path (std::string file) {

	std::ifstream _file (file.c_str (), std::ifstream::in);

	int TERM = m_terminals->getSize();	
	m_host = std::vector < std::vector< PathList > > (TERM, std::vector< PathList >(TERM) );

	for (int i=0; i < m_terminals->getSize (); i++) {
		for (int j=0; j < m_terminals->getSize (); j++) {

			if ( i != j) {
				
				std::string info;
				_file >> info; //begin
				_file >> info; // (
				int paths_size = 0; 
				_file >> paths_size; // quant. paths
				_file >> info; // )

				int t1 = -1;
				int t2 = -1;		
							
				_file >> info; //from
				_file >> t1; //terminal 1
				_file >> info; // (t1)
				_file >> info; // to:
				_file >> t2; //terminal 2
				_file >> info; // (t2)

				for (int k = 0; k < paths_size; k++) {
					
					_file >> info; // [
					rca::Path path;
					std::vector<int> aux;
					do {
						_file >> info; // nodes...
						int v = -1;
						if (info.compare("]") != 0) {
							v = atoi (info.c_str() );
							aux.push_back (v);
						}				
						
					} while (info != "]");	
					double cost = 0.0;
					_file >> cost; // custo			

					std::reverse (aux.begin(), aux.end()); //reversing the path		
					for (unsigned int p = 0; p < aux.size(); p++)
						path.push (aux[p]);
				
					path.setCost (cost);

					m_host[i][j].push_back (path);
											
				}
				_file >> info; //end
			}
		}		
	}	

}
