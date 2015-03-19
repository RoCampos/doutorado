#ifndef _STTREE_OBSERVER_H_
#define _STTREE_OBSERVER_H_

#include <vector>
#include <climits>
#include <algorithm>

#include "steinertree.h"
#include "disjointset2.h"
#include "heapfibonnaci.h"

class SteinerTreeObserver;

/**
 * Esta classe mantém a lista de arestas que estão sendo usandas
 * em uma solução. Com ela é possível controlar a capacidade residual
 * de cada aresta.
 * 
 * @author Romerito Campos
 * @date 03/18/2015
 */
class CongestionHandle {
	friend SteinerTreeObserver;
public:
	
	CongestionHandle (){}
	
	CongestionHandle (rca::Network * net, std::vector<rca::Group> & g){
		m_network = net;
		m_groups = g;
	}	
	
	CongestionHandle & operator=(const CongestionHandle & cong) {
	
		if (this != &cong) {
		
			m_network = cong.m_network;
			m_groups = cong.m_groups;
			m_used_links = cong.m_used_links;
		
		}
		
		return *this;
	}
	
	/**
	 * Método registrar que uma aresta está sendo utilizada pelo grupo group.
	 * 
	 * Este método leva O(E).
	 */
	void add_edge (int i,int j,int group) {
	
		int trequest = m_groups[group].getTrequest ();
		rca::Link link (i,j,0);
		
		std::vector<rca::Link>::iterator it;
		it = std::find (m_used_links.begin (), m_used_links.end(),link);
		
		if (it != m_used_links.end()) {
			
			it->setValue ( it->getValue () - trequest);
			
		} else {
			
			int band = m_network->getBand (link.getX(), link.getY());
			link.setValue (band - trequest);
			m_used_links.push_back (link);
		}
	}

	/**
	 * Este método é utilizado para informar que uma aresta não está sendo
	 * utilizada pelo grupo group.
	 * 
	 * O(E).
	 */
	void remove_edge (int i, int j, int group) {
		
		int trequest = m_groups[group].getTrequest ();
		rca::Link link (i,j,0);
		
		std::vector<rca::Link>::iterator it;
		it = std::find (m_used_links.begin (), m_used_links.end(),link);
		
		if (it != m_used_links.end()) {
			it->setValue ( it->getValue () + trequest);
			
			if (it->getValue () == 0) {
				m_used_links.erase (it);
			}
		}
		
		
		
	}
	
	int getResidualCapacity (rca::Link & l) {
	
		auto it = std::find(m_used_links.begin (), m_used_links.end(), l);
		
		int n = INT_MIN;
		return (it != m_used_links.end() ? it->getValue() : n);
	}
	
	/**
	 * Retorna a capacidade residual mínima
	 * 
	 * O( sort ) + 1; 
	 */
	int congestion () {
		
		std::sort (m_used_links.begin(), m_used_links.end());
		
		if (m_used_links.size () == 0)
			return INT_MIN;
		
		return ( std::min_element (m_used_links.begin(), 
								 m_used_links.end())  )->getValue();
	}
	
	const std::vector<rca::Link> & getUsedLinks () {
		return m_used_links;
	}
	
private:
	//std::vector<std::vector<int>> m_congestion;
	rca::Network * m_network;
	std::vector<rca::Group> m_groups;
	
	std::vector<rca::Link> m_used_links;
	
};

class SteinerTreeObserver {
	
public:
	
	//receive a steiner tree and Observer
	SteinerTreeObserver (SteinerTree* st, CongestionHandle * ch) {
		//m_st = st;
		m_ch = ch;
		
		if (st) {
			setTree (st);
		}
		
		m_cost = 0;
		
	}
	
	void setTree (SteinerTree * st) {
		
		m_st = st;
		
		dset = NULL;
		if (dset) {
			delete dset;
		}
		
		dset = new DisjointSet2 (m_ch->m_network->getNumberNodes ());
	
		
	}
	
	bool addEdge (int i,int j, int value, int group) {
		
		//verifica se está no mesmo conjunto
		if ( dset->find2 (i) != dset->find2 (j) ) {
			
			//faz união
			dset->simpleUnion (i, j);
			rca::Link l (i,j, value);
			int cost = (int)m_ch->m_network->getCost (l.getX(),l.getY());
			
			bool ret = m_st->addEdge (l.getX(), l.getY(), cost);
		
			assert (cost == value);
			if (ret) {
				//increase the cost
				m_cost += cost;
				//compute usage
				m_ch->add_edge (i,j,group);
			}
			
			return true;
		}
		return false;
	}
	
	int getCost () {
		return this->m_cost;
	}
	
	const std::vector<rca::Link> & getTreeAsLinks (){
		std::vector<rca::Link> tree_links;
		return tree_links;
	}
	
	
	void prunning (int group) {
		
		Node *n = m_st->leafs.first ();
	
		while (n != NULL) {
			
			//remover v,w remove v.
			int v = n->value;
			int w = m_st->vertexList[v].getFirstAdjc ();
					
			//recupera nó _n de v
			Node *_n = m_st->vertexList[v].getNode();
			//remove da lista
			
			m_st->vertexList[v].setNode( m_st->leafs.remove (_n) );
					
			//buscar aresta para remoção
			Edge * edge = m_st->vertexList[v].getEdge_ ();
			//atualizar custo
			
			if (edge) {
				m_cost -= edge->cost;
				m_ch->remove_edge (edge->i, edge->j, group);
			}
			
			//remover aresta
			edge = m_st->listEdge.remove (edge);
			m_st->vertexList[v].setEdge_ (edge);
			//se v é aresta de W.
			if (m_st->vertexList[w].getFirstAdjc() == v)
				m_st->vertexList[w].setEdge_ (edge);
					
			//decrementa degree
			m_st->vertexList[w].decreaseDegree ();
			m_st->vertexList[v].decreaseDegree ();
			
			//se w é terminal
			if (m_st->vertexList[w].isTerminal ()) {
				//volta para o início da lista
				n = m_st->leafs.first ();
				
			} else {
				//como w não é terminal,
				//adiciona w na lista
				if (m_st->vertexList[w].getDegree() == 1) {
					_n = NULL;
					_n = m_st->leafs.addFirst (w);
					m_st->vertexList[w].setNode (_n);
					
					n = m_st->leafs.first ();
				} else {
					n = m_st->leafs.first ();
				}
			}
			
		}
		
	}
	
private:
	SteinerTree * m_st;
	CongestionHandle * m_ch;
	int m_cost;
	
	DisjointSet2 * dset;
	
	
};

rca::Path capacited_shortest_path (int v, int w, 
								   rca::Network *network,
								   CongestionHandle *cg,
								   rca::Group &g)
{

	typedef FibonacciHeapNode<int,double> Element; //todo VErificar se é double ou int 1

	int NODES = network->getNumberNodes();

	vector< int > prev = vector< int >( NODES );
	vector< double >distance = vector< double >( NODES ); //acho que tinha um erro aqui
	vector< bool > closed = vector< bool >( NODES );

	FibonacciHeap < int, double > queue; // todo VErificar se é double ou int 2
	Element* elem = NULL;
	int register i;
	for (i=0; i < NODES; i++) {
		prev[i] = -1;
		distance[i] = 9999999;
		if (i == v)
			elem = queue.insert (i,9999999);
		closed[i] = false;
	}

	distance[v] = 0;
	queue.decreaseKey (elem,0);

	double cost = 0.0;
	bool removed = false;
	while (!queue.empty()) {

		int e = queue.minimum ()->data ();
		queue.removeMinimum ();

		closed[e] = true;

		//i is defined as register variable
		for (i=0; i < NODES; ++i) {

			
			cost = network->getCost (e, i);
			
			if (cost > 0.0) {
				
				rca::Link l (e,i,cost);
				removed = network->isRemoved(l);
			
				//getting the current capacity
				int cap = cg->getResidualCapacity (l);
				int band = network->getBand (l.getX(), l.getY());
				//updating the capacity if the link is used
				if (cap == INT_MIN){		
					if (band - g.getTrequest() > 0) {
						cap = 0;
					}
				}else {
					cap -= g.getTrequest();
				}
				
				if (!closed[i] && cost > 0
					&& (distance[i] > (distance[e] + cost))
					&& !network->isVertexRemoved(i) 
					&& !removed
					&& cap >= 0) 
				
				{
					distance[i] = distance[e] + cost;
					prev[i] = e;
					queue.insert (i, distance[i]);
				}
			}
			
		}
		
	}

	delete elem;

	rca::Path path;
	int pos = 0;
	double pathcost = 0.0;
	int s = w;
	while (s != v) {
		path.push (s);
		s = prev[s];
		
		if (s == -1 || s >= NODES)
		{
			rca::Path path2;
			return path2; //se não há caminho
		}
		
		
		pathcost += network->getCost (path[pos],s);
		
		pos++;

	}
	path.push (s);
	path.setCost (pathcost); //definindo o custo
	
	return path;
	
}

#endif
