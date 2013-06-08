#ifndef _READFILE_H_
#define _READFILE_H_

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

//adiciona resultas ao final do arquivo
//o arquivo de saide consiste de uma coluna com os
//resultados. Não usa separador.
void read_file (std::string input,std::string ouput);

//generate a file with the avarage for n runs of an 
//algorithm or a configuration
void average (std::string input, std::string output);

//generate a file with the median for n runs of an 
//algorithm or a configuration
void median (std::string input, std::string output, int sample);

#endif 



