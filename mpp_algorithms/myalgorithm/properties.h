#ifndef _GLOBAL_PROPERTIES_H_
#define _GLOBAL_PROPERTIES_H_

#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include "network.h"
#include "group.h"
#include "reader.h"

//variaveis compartilhadas pelos algoritmos
//otimizar acesso aos conteúdos

extern std::shared_ptr<rca::Network> g_network;
extern std::vector<std::shared_ptr<rca::Group>> g_groups;
  
void init (std::string file);

#endif /*_GLOBAL_PROPERTIES_H_*/