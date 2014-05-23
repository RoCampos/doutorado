#include <iostream>
#include <string>
#include <fstream>
#include <cctype>
#include <sstream>
#include <cstdio>

namespace rca {

#ifndef LINE_SIZE
#define LINE_SIZE  256
#endif
	
#ifndef _FILTER_SOL_
#define _FILTER_SOL_

/**
* Esta classe é utilizada para filtrar uma solução gerada pelo otimizador glpsol.
* A solução final tem o seguinte formato:
* 
* 
* 
*/
class FilterSol {

public:
	FilterSol (std::string, std::string output);
	
	/*Método que realiza o filtro no arquivo passado como parâmetro*/
	void doFilter ();
	
private:
	void getVariable (std::string str);
	
private:
	std::string str_file;
	std::string str_output_file;
	std::ofstream output_file;
	
};	

#endif
};
