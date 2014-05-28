#include "chenalgorithm.h"

using namespace std;

Chen::Chen (std::string instance, std::string _input) {
	
	input = _input;
	
}

void Chen::init () {
	
	Reader reader (instance);
	reader.configNetwork ( net.get () );
	
	
}

void Chen::pre_processing () {
	
	
}