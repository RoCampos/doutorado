#include <iostream>

#include "properties.h"

int main (int argv, char **argc) {
 
  //method from properties to init the 
  //variables
  rca::init_properties (argc[1]);
  
  return 0;
}