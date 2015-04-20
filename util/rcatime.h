#ifndef _MY_UTIL_H_
#define _MY_UTIL_H_

#include <chrono>

namespace rca {

struct elapsed_time {

	std::chrono::high_resolution_clock::time_point m_start;
	std::chrono::high_resolution_clock::time_point m_finish;
	
	void started ();
	void finished ();
	
	double get_elapsed ();
		
};

};

#endif 
