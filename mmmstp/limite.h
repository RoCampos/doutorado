#ifndef _LIMITS_H_
#define _LIMITS_H_

#include <vector>
#include <memory>

#include "link.h"
#include "group.h"
#include "network.h"
#include "reader.h"
#include "sttree.h"
#include "edgecontainer.h"
#include "steiner_tree_observer.h"

/**
 * Computes a steiner tree for each group. The algorithm build
 * a spanning minimal tree for each group and make prune on non-terminal
 * leaf nodes. The algorithm do not consider the residual capacity contraint.
 * 
 * The value could be a good lower bound for big instances.
 * 
 * @param std::vector<rca::Group>
 * @param rca::Network
 */
double kruskal_limits (std::vector<rca::Group> &, rca::Network &);

#endif /*_LIMITS_H_*/