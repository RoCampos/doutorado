#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_

#include <vector>
#include <stack>

#include "path.h"
#include "network.h"
#include "group.h"

template <int option = 0>
rca::Path shortest_path (int v, int w, rca::Network * network);

bool is_connected (rca::Network & network, rca::Group & group);

//explit instantiation
//template rca::Path shortest_path<0> (int v, int w, rca::Network * network);

#endif