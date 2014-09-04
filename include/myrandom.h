#include <iostream>
#include <random>
#include <chrono>

namespace rca {

/**
 * This template struct is used to facilite the manipulation
 * of random number generation.
 * 
 * It is possible use different types of random number generators
 * provided by random lib of c++ 11.
 * 
 * @author Romerito Campos
 * @version 1.0
 * @date 09/03/2014
 */
template<class Engine, class Dist>
struct myrandom {

	Engine engine;
	Dist distribution;
	
	myrandom (long seed, int a, int b) {
		
		//initializing the engine
		engine = Engine (seed);
		
		//initializing the distribution object
		distribution = Dist (a, b);
	}
	
	int rand () {
		int w = distribution(engine);
		return w;
	}
	
};

struct myseed {

	static long seed () {		
		long value = std::chrono::system_clock::now (). time_since_epoch (). count ();
		return value;
	}
	
};
	
}