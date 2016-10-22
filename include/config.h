#ifndef CONFIG_H
#define CONFIG_H


#include <cstdio>
#include <cstdlib>

#define VERSION_MAJOR 2
#define VERSION_MAJOR 0
#define VERSION_PATCH 0

void message (int argc, char const *argv[]) 
{
	if (argc < 2) {
		fprintf(stdout,"%s Version %d.%d.%d\n",
			argv[0],
			VERSION_MAJOR,
			VERSION_MAJOR,
			VERSION_PATCH);
		fprintf(stdout,"Usage: %s number\n",argv[0]);
	}
}

#endif // CONFIG_H
