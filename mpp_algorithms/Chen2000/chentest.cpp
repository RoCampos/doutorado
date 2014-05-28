#include <iostream>

#include "chenalgorithm.h"

int main (int argc, char**argv) {
	
	Chen c(argv[1],argv[2]);
	
	int max = c.get_max_congestion ();
	
	cout << max << endl;
	
	return 0;
}