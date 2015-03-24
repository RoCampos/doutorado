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
template<class Engine, class Dist, typename TypeValue>
struct myrandom {

	Engine engine;
	Dist distribution;
	
	myrandom(){}
	
	myrandom(const myrandom<Engine, Dist, TypeValue> & ref) {
		
		engine = ref.engine;
		distribution = ref.distribution;
	}
	
	myrandom (long seed, TypeValue a, TypeValue b) {
		
		//initializing the engine
		engine = Engine (seed);
		
		//initializing the distribution object
		distribution = Dist (a, b);
	}
	
	myrandom (const Engine & e, TypeValue a, TypeValue b) {
		engine = e;
		distribution = Dist (a, b);
	}
	
	void reset_interval (TypeValue a, TypeValue b) {
		distribution = Dist (a,b);
	}
	
	TypeValue rand () {
		TypeValue w = distribution(engine);
		return w;
	}
	
	const Engine& get_engine () const{
		return engine;
	}
	
};

/**
 * This struct is used to generate 
 * a seed to the number generator.
 * 
 * @author Romerito Campos.
 * @date 09/04/2014
 */
struct myseed {

	static long seed () {		
		long value = std::chrono::system_clock::now (). time_since_epoch (). count ();
		return value;
	}
	
};
	
}

template class rca::myrandom<std::mt19937, std::uniform_int_distribution<int>, int>;
template class rca::myrandom<std::mt19937, std::uniform_real_distribution<double>, double>;