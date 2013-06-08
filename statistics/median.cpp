#include <iostream>
#include <fstream>

#include "readFile.h"

using namespace std;

int main (int argc, char** argv) {

	int sample = atoi (argv[3]);
	median ( argv[1], argv[2], sample);

	return 0;
}
