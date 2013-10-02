#ifndef _HEURISTIC_H_
#define _HEURISTIC_H_

#include "steinertree.h"
#include "network.h"
#include "group.h"
#include "path.h"
#include "algorithm.h"
#include "kspath.h"

#include <fstream>

typedef std::vector<rca::Path> PathList;

extern rca::Network * m_network;
extern rca::Group * m_terminals;
extern std::map<int,int> m_term_index; //mapeia do nó real para index
extern std::vector<SteinerNode> _nodes;
extern std::vector<double> objs;

//caminho entre os terminais, banco de informação
extern std::vector< std::vector< PathList > > m_host;

//inicializa as variáveis do algoritmo
void init (rca::Network * , rca::Group * );

//inicializa a population, cria as estruturas
void initPopulation (int size, std::vector<SteinerTree> &);

//incializa uma solução apenas.
void initSolution (SteinerTree *);

//heuristic to create a solution
void createSolution (SteinerTree *);

//cria uma solução a partir de uma lista ordenada de arestas.
void createSolution (SteinerTree &, std::vector<rca::Link>&);

//get the edges from this individual st
void getEdgesFromIndividual (SteinerTree & st, std::vector<rca::Link>&);

//get the edges from 
void getEdgesFromPaths (int t1, int t2, int path, std::vector<rca::Link>&);

//get all paths stored in the set end copyt o vector.
void getPathsFromSet (std::set<rca::Path> &, std::vector<rca::Link> &);

//cria matriz terminal x terminal, onde cada celular
//armazena uma lista de caminhoa entre os terminais
void initHostInfo (int K);

//Este método cria n_paths entre t1, t2.
//isto é feito sob demanda, durante a execução do algoritmo
void addHostInfo (int t1, int t2, int path_size);

//store and read paths from a file
void write_path (std::string file); //write to a file
void read_path (std::string file); //read from a file

#endif
