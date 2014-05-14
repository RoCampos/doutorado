#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include <sstream>

namespace rca {

#ifndef LINE_SIZE
#define LINE_SIZE  256
#endif
	
#ifndef _FILTER_SOL_
#define _FILTER_SOL_
class FilterSol {

public:
	FilterSol (std::string);
	void doFilter ();
	
private:
	void getVariable (char str[]);
	
private:
	std::string str_file;
	
};	

#endif
};