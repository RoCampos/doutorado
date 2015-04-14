#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>

using namespace std;

typedef typename std::vector<double> table_t;

table_t get_table (ifstream & file) {
	
	int min;
	double seconds;

	table_t table;

	std::string line;
	char str[4];
	char user[] = "user";
	while ( getline (file, line) ) {
		if ( sscanf ( line.c_str (), "%s %dm%lfs",&str, &min, &seconds) != 0) {
			//printf ("%lf\n", seconds*1000);
			if (strcmp (str, user) == 0)
				table.push_back (seconds*1000);
		}
	}

	return table;
}

void column_bind (table_t const &t1, table_t const &t2) {

	if (t1.size () != t2.size ()) {
		printf ("The tables has different sizes");
		exit (1);
	}	

	std::cout << "Table size: " << t1.size () << std::endl;

	for (size_t i=0; i < t1.size (); i++) {
		printf ("%lf\t%lf\n", t1[i], t2[i]);
	}

}

int main (int argc, char** argv) {

	
	std::string fib = argv[1];
	std::string old_fib = argv[2];

	std::ifstream file_fib (fib.c_str());
	std::ifstream old_file_fib (old_fib.c_str());

	table_t fib_table = get_table (file_fib);
	table_t old_fib_table = get_table (old_file_fib);

	column_bind (fib_table, old_fib_table);

	file_fib.close ();
	old_file_fib.close ();

	return 0;
}
