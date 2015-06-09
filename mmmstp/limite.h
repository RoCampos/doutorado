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

double kruskal_limits (std::vector<rca::Group> &, rca::Network &);

#endif /*_LIMITS_H_*/