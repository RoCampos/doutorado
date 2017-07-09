#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#include <vector>
#include <stack>

#include "path.h"
#include "network.h"
#include "group.h"
#include "heapfibonnaci.h"
#include <boost/heap/fibonacci_heap.hpp>
#include <boost/heap/binomial_heap.hpp>
#include "edgecontainer.h"


typedef struct vertex_t {
	
	int weight;
	int id;
	bool used;
	
	vertex_t (int _w, int _id) : weight(_w), id(_id){}
	
	bool operator < (const vertex_t & ref) const{
		return weight < ref.weight;
	}
	
	bool operator > (const vertex_t & ref) const{
		return weight > ref.weight;
	}
	
}vertex_t;

typedef typename boost::heap::fibonacci_heap<vertex_t,boost::heap::compare<std::less<vertex_t>>>::handle_type handle_t;
typedef typename rca::EdgeContainer<rca::Comparator, rca::HCell> CongestionHandle;


// for max_heap
struct compare_greater {
	bool operator () (vertex_t const& t1, vertex_t const& t2) const{
		return t1.weight > t2.weight;
	}
};

typedef typename boost::heap::compare<compare_greater> gcomparator_t;
typedef typename boost::heap::fibonacci_heap<vertex_t, gcomparator_t>::handle_type g_handle_t;
typedef typename boost::heap::fibonacci_heap<vertex_t, gcomparator_t> fibonacci_greater_t;


/**
* Algoritmo de caminho mais curto utilizando Dijkstra.
* Este algoritmo encontra o caminho mais curto entre dois
* vértices v e w. Após encontrar o caminho, mais curto o mesmo
* é retornado.
*
* Uma implementação de heap é utilizada para aumentar a velocidade
* do algoritmo. Detalhe: a implementação utilizada não é a implementação
* fornecida pela boost graph library.
*
*
* @param int vértice v
* @param int vértice w
* @param rca::Network - ponteiro
* @return rca::Path
*/
rca::Path shortest_path (int v, int w, rca::Network * network);


/**
* Algoritmo de caminho mais curto utilizando Dijkstra.
* Este algoritmo encontra o caminho mais curto entre dois
* vértices v e w. Após encontrar o caminho, mais curto o mesmo
* é retornado.
*
* Esta versão utiliza a implementação do heap de fibonacci fornecido
* pela boost graph library.
*
* Para aumentar a velocidade do algoritmo, quando se encontra o alvo w
* o loop principal do algoritmo é encerrado e o caminho de v para w é
* constituído.
*
* @param int vértice v
* @param int vértice w
* @param rca::Network - referência
* @return rca::Path
*/
rca::Path shortest_path (int v, int w, rca::Network & network);

/**
* Algoritmo de caminho mais curto utilizando Dijkstra.
* Este algoritmo encontra o caminho mais curto entre dois
* vértices v e w. Após encontrar o caminho, mais curto o mesmo
* é retornado.
*
* Esta versão utiliza a implementação do heap de fibonacci fornecido
* pela boost graph library.
*
* Para aumentar a velocidade do algoritmo, quando se encontra o alvo w
* o loop principal do algoritmo é encerrado e o caminho de v para w é
* constituído.
*
* @param int vértice v
* @param int vértice w
* @param rca::Network - referência
* @param std::vector<int> costpath,
* @param std::vector<std::vector<int>>  paths
* @return rca::Path
*/
void shortest_path (int v, 
	rca::Network & network, 
	std::vector<int> &bases, 
	std::vector<int> &costpath, 
	std::vector<std::vector<int>> & paths);

/**
* Este procedimento é utilizado para fazer uma árvore de busca em 
* profundidade utilizando o parâmetro 'src' como raíz da busca.
*
* Ele deve retornar o uma lista de caminhos ligando dos os nós
* a fonte e os caminhos que fazem a ligação.
*
* @param int vértice source - src
* @param rca::Network - referência
* @param std::vector<int> costpath,
* @param std::vector<std::vector<int>>  paths
* @return rca::Path
*/
void best_first_search (int src,
	rca::Network const & network, 
	std::vector<int> &bases, 
	std::vector<int> &costpath, 
	std::vector<std::vector<int>> & paths);


/**
* Algoritmo de caminho aumentado que gera uma árvore de caminho
* aumentado da fonte para cada vértice de destino.
*
* Este algoritmo gera uma lista de caminhos da fonte para
* os demais vértices da árvore considerando o caminho aumentado,
* ou seja, caminho com mais capacidade residual.
*
*
*
* @param int vértice v
* @param rca::Network - referência
* @param std::vector<int> costpath,
* @param std::vector<std::vector<int>>  paths
* @return rca::Path
*/
void widest_shortest_path (
	int source,
	rca::Network & network,
	std::vector<int> & bases,
	std::vector<int> & costpath,
	std::vector<std::vector<int>> & paths);

/**
* Algoritmo de caminho mais curto que utiliza o heap de fibonacci.
* Porém, a implementação utilizada não é a implementação fornecida
* pela boost graph library.
* 
* O algoritmo não para o loop principal de Dijkstra quando encontra
* o alvo w. No entanto, os caminhos mais curtos de v para todos os outros
* nós do grafo são computados.
* 
* @param int vértice v
* @param int vértice w
* @param rca::Network - ponteiro
* @return rca::Path
*/
std::vector<int> inefficient_widest_path (int v, int w, rca::Network * network);

/**
* Este algoritmo computa o caminho mais curto entre os nós v e w.
* Ele considera o caminho mais curto com a restrição de que as 
* arestas utilizadas para criar o caminho devem satisfazer o limite
* de capacidade.
*
* A implementação utilizada é a mesma que @see inefficient_widest_path.
*
*
* @param int vértice v
* @param int vértice w
* @param rca::Network - ponteiro
* @param CongestionHandle
* @return rca::Path
*
*/
rca::Path capacited_shortest_path (int v, int w, 
								   rca::Network *network,
								   CongestionHandle *cg,
								   rca::Group &g);

/**
* Este algoritmo verifica se os nós do grupo multicast estão 
* conectados entre si.
* 
* Logo, pode haver algum nó da rede que não esteja conectado
* aos demais nós. No entanto, está versão do algoritmo faz o 
* teste considerando apenas os nós do grupo multicast (fonte/destinos).
*
* Este algoritmo é empregado quando se tem interesse em fazer modificações
* no grafo que representa a rede do problema. Assim, garantindo que os nós
* fonte e destinos estejam sempre conectados.
*
* @param rca::Network
* @param rca::Group
*/
bool is_connected (rca::Network & network, rca::Group & group);

/**
 * Este algoritmo é utilizad para montar um caminho entre o nó v e o nó w. 
 * Ele é executado após se aplicar o algoritmo de caminho mais curto @see all_shortest_path.
 * 
 * O algoritmo @see all_shortest_path faz a computação do caminho mais curto de v para todos 
 * os nós do grafo.
 * 
 * O algoritmo vai receber os dois nós fonte/alvo, a rede para obteção de custos e a lista 
 * de predecessores que é obtida pelo algoritmo de caminho mais curto.
 *
 * @param int v
 * @param int w
 * @param rca::Network
 * @param std::vector<int> predecessors
 * @return rca::Path
 */
rca::Path get_shortest_path (int v, int w, 
						rca::Network & network, 
						std::vector<int> & prev);

/**
* Este algoritmo computa o caminho mais curto de v para todos os nós
* em no grafo.
*
* Ele utiliza o heap de fibonacci que faz parte da boost graph library.
* O algoritmo retorna um lista de predecessores para que os caminhos possam ser estraídos.
* 
* @param int v
* @param int w
* @param rca::Network
* @return std::vector<int> predecessors
*/
std::vector<int> all_shortest_path (int v, int w, rca::Network & network);


/**
* This algorithm is used to compute the voronoi diagram
* of a graph based on maximun residual capacity of edges
* of the graph passed as parameters.
*
* The algorithm creates a artificial source and connect it with
* the nodes passed as parameter 'sources'.
*
* After run dijkstra algorithm (modified version), the edges that
* connect the artificial source to the network are cutted off. So,
* some connected components are connected using a edge (based on cost or
* residual capacity).
*
* This algorithm can be used to build network distance of a graph
*
*
* @param vector<int> the sources
* @param rca::Network the network where the tree is built
*
*/
void voronoi_diagram (
	rca::Network const &,
	std::vector<int> & bases,
	std::vector<int> & costpath,
	std::vector<std::vector<int>> & paths);

void widest_shortest_path (
	int source,
	rca::Network & network,
	std::vector<int> & bases,
	std::vector<int> & costpath,
	std::vector<int> & prev,
	std::vector<std::vector<int>> & paths);


/**
* Retorna o menor valor de capacidade dentre todas as arestas da rede.
* 
*
* @param rca::Network
* @return double
*/
double min_bandwidth (rca::Network& network);

/**
*	Este método é utilizado para pegar a aresta de um caminho
* com menor valor de capacidade residual e retoná-lo.
* 
*
* @param rca::Network
* @param rca::Path
*/
int get_bottleneck (
	rca::Network& network, 
	rca::Path& path);

/**
*	Este método é utilizado para transformar um rca::Path
*	em um conjunto de arestas adicionando o custo da aresta
* 	obtido da rede, que e passada como parâmetro.
*
*
*	@param rca::Path
*	@param rca::Network
*	@return std::vector<rca::Link>
*/
std::vector<rca::Link> path_to_edges (rca::Path const& path, 
	rca::Network * net = NULL);

#endif