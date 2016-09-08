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
* Retorna o menor valor de capacidade dentre todas as arestas da rede.
* 
*
* @param rca::Network
* @return double
*/
double min_bandwidth (rca::Network& network);


std::vector<rca::Link> path_to_edges (rca::Path const& path, 
	rca::Network * net = NULL);

#endif