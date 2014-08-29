#ifndef _ACO_RUN_
#define _ACO_RUN_

#include <iostream>

#include "network.h"
#include "steinertree.h"

namespace rca {

template <typename AcoHandle>
class ACO : private AcoHandle {

public: 
	void run ();
	
};

}

#endif /*_ACO_RUN_*/