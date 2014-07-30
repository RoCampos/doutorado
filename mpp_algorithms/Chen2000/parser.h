#ifndef _FILTRO_PBD__
#define _FILTRO_PBD__

#include <iostream>
#include <fstream>
#include <limits>
#include <string.h>
#include <string>

#include <boost/regex.hpp>

/**
 * This class is a parser for results obtained from
 * optimizer like: gurobi, glpk.
 * 
 * This parser is used only for steiner tree results.
 * 
 * @author Romerito Campos.
 * @version 0.1
 * @date 30/07/2014
 */
template <typename Optimizer>
class Parser :  private Optimizer
{
 
  using Optimizer::filter_sol;
  
public:
  void run (std::string input, std::string output) const {
   
    filter_sol (input, output);
    
  }
  
};

/**
 * Class that filter the informations about edges in Steiner
 * tree obtained from Gurobi optimizer.
 * 
 * This class implements a method called filter_sol used
 * to filter the solution.
 * 
 * The resulting content is stored in a file.
 * 
 * @author Romerito Campos
 * @version 0.1
 * @date 30/07/2014
 */
class Gurobi
{
    
protected:
  /** 
   * This method filter a gurobi output for steiner tree and
   * store the resulting steiner tree in file.
   * 
   * @param std::string a input file containing gurobi output
   * @param std::string a output file containing the steiner tree.
   */
  void filter_sol (std::string input, std::string output) const;
  
};

int main (int argv, char**argc) {
 
  Parser<Gurobi> parser;  
  parser.run (argc[1], argc[2]) ;
  
}

#endif