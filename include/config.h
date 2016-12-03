#ifndef CONFIG_H
#define CONFIG_H


#include <cstdio>
#include <cstdlib>

#define VERSION_MAJOR 2
#define VERSION_MINOR 3
#define VERSION_PATCH 0

bool message (int argc, char const *argv[], std::string command) 
{
	if (argc < 2) {
		fprintf(stdout,"%s Version %d.%d.%d\n",
			argv[0],
			VERSION_MAJOR,
			VERSION_MINOR,
			VERSION_PATCH);
		fprintf(stdout,"Usage: %s %s\n",argv[0], command.c_str ());
		return true;
	}

	return false;
}

#endif // CONFIG_H
