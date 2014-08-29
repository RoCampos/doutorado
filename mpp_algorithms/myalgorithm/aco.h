#ifndef _ACO_RUN_
#define _ACO_RUN_

#include <iostream>
#include <memory>
#include <string>

#include "network.h"
#include "group.h"
#include "steinertree.h"

namespace rca {

template <typename AcoHandle>
class ACO : private AcoHandle {

using AcoHandle::update_pheromone;
using AcoHandle::solution_construction;
using AcoHandle::initialization;
using AcoHandle::configurate;

public:
	void run ();
	inline void set_instance (std::string & instance) {
		m_instance = instance;
	}
	
private:
	std::string m_instance;
	
};

}

#endif /*_ACO_RUN_*/