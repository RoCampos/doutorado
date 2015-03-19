#include "steinertree.h"

SteinerTree::SteinerTree (int nobj, int nnodes)
: num_obj(nobj), num_nodes(nnodes)
{
	
}

SteinerTree::SteinerTree(int nnodes) 
: num_obj(1), num_nodes(nnodes)
{
  objectives = std::vector<double>(num_obj);
  
  vertexList = std::vector<SteinerNode>(num_nodes, 
					SteinerNode(0,0,false));
  for (int i=0; i < num_nodes; i++) {
    vertexList[i].setIndex(i);
  }
}

SteinerTree::SteinerTree (int nnodes, 
						  int source, 
						  const std::vector<int> & terminals)
: num_obj (1), num_nodes (nnodes)
{

	objectives = std::vector<double>(num_obj);
	vertexList = std::vector<SteinerNode> (num_nodes,SteinerNode(0,0,false));
	
	for (int i=0; i < num_nodes; i++) {
		vertexList[i].setIndex (i);
	}
	
	//defining the destinations as terminals
	for (unsigned i = 0; i < terminals.size (); i++) {
		setTerminal ( terminals[i] );
	}
	//adding the source to termainl's group
	setTerminal (source);
	
}

//this procedure takes O(n) since we must avoid duplicated edges.
bool SteinerTree::addEdge (int i, int j, double value)
{
	if (vertexList[i].getFirstAdjc () == j ||
		vertexList[j].getFirstAdjc () == i)
	{
		return false;
	}
	
	if (listEdge.exist (i,j)) {
	  return false;
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
	
	return true;
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
		
		printf ("%d--%d : %.3f\n",v,w,value);
		
		aux = aux->next;
		
	}
	printf ("}\n");
	
}

void SteinerTree::xdotToFile (std::string file)
{

	std::ofstream output (file.c_str(), std::ofstream::out);
	
	
	output << "Graph {\n";
	//printf ("Graph{\n");
	
	for (unsigned int i = 0; i < vertexList.size (); i++) {
		if (vertexList[i].isTerminal() ) {
			//printf ("%d [format=circle,color=red];\n",i);
			output << i << "[format=circle,color=red];\n";
		}
	}
	
	//imprimindo arestas
	Edge *aux = listEdge.head;
	while (aux != NULL) {
		
		int v = aux->i;
		int w = aux->j;
		double value = aux->cost;
		
		//printf ("%d--%d : %.2f\n",v,w,value);
		output << v;
		output << "--";
		output << w;
		output << " : ";
		output << std::setprecision(3) << value << std::endl;
		
		aux = aux->next;
		
	}
	
	//printf ("}\n");
	output << "}\n";
	
}
