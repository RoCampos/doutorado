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

using AcoHandle::run;
using AcoHandle::configurate;
using AcoHandle::configurate2;

public:
	void run ();
	inline void set_instance (std::string & instance) {
		m_instance = instance;
	}
	inline void set_iterations (int it) {
		m_iterations = it;
	}
	
private:
	std::string m_instance;
	int m_iterations;
};

}

#endif /*_ACO_RUN_*/