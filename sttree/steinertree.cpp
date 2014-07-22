#include "steinertree.h"

SteinerTree::SteinerTree (int nobj, int nnodes)
: num_obj(nobj), num_nodes(nnodes)
{
	
}

void SteinerTree::setTempStructures (std::vector<double> & objs, 
				std::vector<SteinerNode> & _nodes)
{
	objectives = objs;
	vertexList = _nodes;
}

void SteinerTree::addEdge (int i, int j, int value)
{
	if (vertexList[i].getFirstAdjc () == j ||
		vertexList[j].getFirstAdjc () == i)
	{
		return;
	}
	
#ifdef DEGUB
  std::cout << i << "-" << j;
  std::cout << ": gfa(" << i << ")=" << vertexList[i].getFirstAdjc()<< std::endl;
  std::cout << ": gfa(" << j << ")=" << vertexList[j].getFirstAdjc() << std::endl;
#endif
	
	vertexList[i].increaseDegree ();
	vertexList[j].increaseDegree ();
	
	Edge * edge = listEdge.add (i,j,value);
	
	objectives[0]+= value;
	
	//se i não é terminal
	if (!vertexList[i].isTerminal()) {
		//se i é folha então
		//defina j como aresta de i.
		//(i,j) remove i da árvore
		if ( !updateLeafs (i) ) {
			vertexList[i].setFirstAdjc (j);
			vertexList[i].setEdge_ (edge);
		}
			
	}
	
	//o mesmo que foi feito para i
	if (!vertexList[j].isTerminal()) {
		if ( !updateLeafs (j) ) {
			vertexList[j].setFirstAdjc (i);
			vertexList[j].setEdge_ (edge);
		}
	}	
	
	edge = NULL;
}

/**
 * Método recebe os indices dos nós adicionados 
 * a lista. Este método atualiza os nós folhas
 * removendo e adiciona nós a lista de folhas
 */
bool SteinerTree::updateLeafs (int i) {
	//se o vértice tem grau 1
	//adicione ele a lista de folhas
	if (vertexList[i].getDegree() == 1) {
					
		Node *n = leafs.addLast (i);
		vertexList[i].setNode (n);
		
		return false;
		
	//se o vértice ficou com grau dois então 
	//remove ele da lista de folhas
	} else if (vertexList[i].getDegree () > 1) {
		
		Node *n = vertexList[i].getNode ();
		vertexList[i].setNode(leafs.remove (n) );
		n = NULL;
		
		return true;
	}
	
}

void SteinerTree::prunning () {
	
	Node *n = leafs.first ();
	
	while (n != NULL) {
		
		//remover v,w remove v.
		int v = n->value;
		int w = vertexList[v].getFirstAdjc ();
				
		//recupera nó _n de v
		Node *_n = vertexList[v].getNode();
		//remove da lista
		
		vertexList[v].setNode( leafs.remove (_n) );
				
		//buscar aresta para remoção
		Edge * edge = vertexList[v].getEdge_ ();
		//atualizar custo
		
		if (edge) {
			objectives[0] -= edge->cost;			
		}
		
		//remover aresta
		edge = listEdge.remove (edge);
		vertexList[v].setEdge_ (edge);
		//se v é aresta de W.
		if (vertexList[w].getFirstAdjc() == v)
			vertexList[w].setEdge_ (edge);
				
		//decrementa degree
		vertexList[w].decreaseDegree ();
		vertexList[v].decreaseDegree ();
		
		//se w é terminal
		if (vertexList[w].isTerminal ()) {
			//volta para o início da lista
			n = leafs.first ();
			
		} else {
			//como w não é terminal,
			//adiciona w na lista
			if (vertexList[w].getDegree() == 1) {
				_n = NULL;
				_n = leafs.addFirst (w);
				vertexList[w].setNode (_n);
				
				n = leafs.first ();
			} else {
				n = leafs.first ();
			}
		}
		
	}
	
}

void SteinerTree::xdotFormat () {
	
	printf ("Graph{\n");
	
	for (unsigned int i = 0; i < vertexList.size (); i++) {
		if (vertexList[i].isTerminal() )
			printf ("%d [format=circle,color=red];\n",i);
	}
	
	//imprimindo arestas
	Edge *aux = listEdge.head;
	while (aux != NULL) {
		
		int v = aux->i;
		int w = aux->j;
		double value = aux->cost;
		
		printf ("%d--%d : %.2f\n",v,w,value);
		
		aux = aux->next;
		
	}
	printf ("}\n");
	
}
