#ifndef _L_SEARCH_
#define _L_SEARCH_


#include <vector>
#include <set>

#include "network.h"
#include "link.h"
#include "group.h"
#include "edgecontainer.h"
#include "sttree.h"

/**
 * This struct is used to perform local search 
 * on steiner tree. The improvement is based
 * on cost and some constraints in the network, 
 * for example newtork with removed edges.
 * 
 */
template<typename Container>
struct cycle_local_search {

	/**
	 * Template method used to perform local search on indivudual multicast trees
	 * to improve the cost of a solution. The procedure is based on creation of
	 * cycles in the multicast tree. This cycle are removed in the way the some
	 * improvement could be found.
	 * 
	 * @param int tree_id where the local search is perfomed
	 * @param std::vector<STTree> vector containing the trees to update the tree
	 * @param rca::Group group multicast related to tree tree_id
	 * @param Container template parameter: @see rca::EdgetContainer
	 */
	void execute ( int tree_id, 
				std::vector<STTree> &, 
				rca::Network& m_network, 
				std::vector<rca::Group>&, 
				Container&);
	
	/**
	 * This method is used to return the cycle where the new edge is added. 
	 * Considering a edge e=(x,w) the cycle is formed e is added a STTree T.
	 * 
	 * So the method find the path between x and w in the STTRee.
	 * 
	 * @param std::vector<rca::Link> STTree as a list of links
	 * @param rca::Group multicast group related to STTree
	 * @param rca::Link that create the cycle in the tree
	 * @param rca::Network where the trees are acommodated
	 */
	std::vector<rca::Link> 
	get_circle (std::vector<rca::Link>&, 
				rca::Group&, 
				rca::Link&, 
				rca::Network &);
	
	/**
	 * This method make calls to execute until some improvement can be found.
	 * 
	 * It receives the same set of parameters pluas and adicional paramete: cost
	 * of solution befora start the local search.
	 * 
	 * @param std::vector<STTree> vector containing the trees to update the tree
	 * @param rca::Group group multicast related to tree tree_id
	 * @param Container template parameter: @see rca::EdgetContainer
	 * @param int cost of solution before local search
	 */
	void local_search (std::vector<STTree> &, 
				rca::Network& m_network, 
				std::vector<rca::Group>&, 
				Container&, 
				int cost);
	
};

#endif