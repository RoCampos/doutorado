#include "mystring.h"

//using namespace rca;

std::string rca::get_instance_name (std::string file) {
	
	size_t end = file.find ('.');
	
	for (size_t i=0; i < end; i++) {
		std::cout << file.at (i);
	}
	cout << endl;
	
	return "";
}