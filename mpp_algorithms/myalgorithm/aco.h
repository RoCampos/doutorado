#ifndef _ACO_RUN_
#define _ACO_RUN_

#include <iostream>
#include <memory>
#include <string>
#include <cstdarg>

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
	void run (int numArgs, std::string instance, ...);
	
private:
	std::string m_instance;
	int m_iterations;
};

}

#endif /*_ACO_RUN_*/