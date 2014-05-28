#include "chenalgorithm.h"

using namespace std;

Chen::Chen (std::string _input) {
	
	input = _input;
	
}

void Chen::pre_processing () {
	
	cout << "pre-processing" << endl;
	cout << input.c_str () << endl;
	ifstream file_ ( input.c_str ());
	
	int x;
	int y;
	while (file_.good()) {
		string str;
		file_ >> x;
		cout << str <<  " ";
		file_ >> str;
		file_ >> str;
		//cout << str << endl;
		for (int i=0; i < str.length (); i++) {
			if (str.at(i))
		}
		
	}
	
}