#ifndef _STTREE_OBSERVER_H_
#define _STTREE_OBSERVER_H_

#include "steinertree.h"

class CongestionHandle {
	
public:
	
	CongestionHandle (){}
	
	CongestionHandle (rca::Network * net, std::vector<rca::Group> & g){
		m_network = net;
		m_groups = g;
		
		int NODES = m_network->getNumberNodes ();
		m_congestion = std::vector<std::vector<int>> (NODES);
		for (int i=0;i < NODES; i++) {
			m_congestion[i] = std::vector<int> (NODES);
		}
		
		for (int i=0; i < NODES; i++) {
			for (int j=0; j < NODES;j++) {
				m_congestion[i][j] = m_network->getBand (i,j);
			}
		}
	}
	
	void add_edge (int i,int j,int group) {
	
		m_congestion[i][j] -= m_groups[group].getTrequest ();
		m_congestion[j][i] = m_congestion[i][j];
		
	}
	void remove_edge (int i, int j, int group) {
		m_congestion[i][j] += m_groups[group].getTrequest ();
		m_congestion[j][i] = m_congestion[i][j];
	}
	
	int congestion () {
		
		int max = 9999;
		for (int i=0; i < (int)m_congestion.size (); i++) {
			for (int j=0; j < (int) m_congestion[i].size(); j++ ) {
				
				if (m_network->getCost (i,j) > 0) {
					
					if (m_congestion[i][j] < max) {
						max = m_congestion[i][j];
					}
				}
				
			}
		}
		return max;
	}
	
private:
	std::vector<std::vector<int>> m_congestion;
	rca::Network * m_network;
	std::vector<rca::Group> m_groups;
	
	
};

class SteinerTreeObserver {
	
public:
	
	//receive a steiner tree and Observer
	SteinerTreeObserver (SteinerTree* st, CongestionHandle& ch) {
		m_st = st;
		m_ch = ch;
		m_cost = 0;
	}
	
	void setTree (SteinerTree * st) {
		m_st = st;
	}
	
	void addEdge (int i,int j, int value, int group) {
		m_st->addEdge (i,j,value);
		m_cost += value;
		
		m_ch.add_edge (i,j,group);
	}
	
	int getCost () {
		return this->m_cost;
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
				m_ch.remove_edge (edge->i, edge->j, group);
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
	CongestionHandle m_ch;
	int m_cost;
};

#endif
