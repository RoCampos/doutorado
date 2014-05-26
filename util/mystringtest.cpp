#include <iostream>
#include "mystring.h"

using namespace std;

int main (int argv, char**argc) {
	
	rca::get_instance_name (argc[1]);
	
	return 0;
}