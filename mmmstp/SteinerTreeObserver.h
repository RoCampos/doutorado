#ifndef _STTREE_OBSERVER_H_
#define _STTREE_OBSERVER_H_

#include "steinertree.h"
#include "disjointset2.h"


class SteinerTreeObserver;

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

	void remove_edge (int i, int j, int group) {
		
		int trequest = m_groups[group].getTrequest ();
		rca::Link link (i,j,0);
		
		std::vector<rca::Link>::iterator it;
		it = std::find (m_used_links.begin (), m_used_links.end(),link);
		
		if (it != m_used_links.end()) {
			it->setValue ( it->getValue () + trequest);
		}
		
	}
	
	int congestion () {
		
		std::sort (m_used_links.begin(), m_used_links.end());
		
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
		m_st = st;
		m_ch = ch;
		m_cost = 0;
		
		dset = NULL;
	}
	
	void setTree (SteinerTree * st) {
		
		if (dset) {
			delete dset;
		}
		
		dset = new DisjointSet2 (m_ch->m_network->getNumberNodes ());
	
		m_st = st;
	}
	
	void addEdge (int i,int j, int value, int group) {
		
		//verifica se está no mesmo conjunto
		if ( dset->find2 (i) != dset->find2 (j) ) {
			
			//faz união
			dset->simpleUnion (i, j);
			int cost = (int)m_ch->m_network->getCost (i,j);
			
			//adiciona aresta a m_st
			(i > j ? m_st->addEdge (i, j, cost):
					 m_st->addEdge (j, i, cost));
		
			//calculate cost
			m_cost += value;
			
			//compute usage
			m_ch->add_edge (i,j,group);
		}
		
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

#endif
