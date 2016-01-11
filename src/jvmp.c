/*******************************
 * file name:   jvm.c
 * description: program entry 
 * date:        2015-01-19
 * author:      kari.zhang
 *
 * modifications:
 *
 * 1. Modified by Kari.Zhang @ 2015-01-19
 *      rearchitect & redesign
 * ****************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "class.h"

#define JVMP_VER "0.0.1"

static void usage();

int main(int argc, char *argv[]) {
	if (2 != argc) {
		usage();
		return -1;
	}

	if (0 == strncmp(argv[1], "-v", 2)) {
		printf("jvmp %s compiled on %s copyright@kari.zhang\n", JVMP_VER, __DATE__);
		return 0;

	} else if (*argv[1] == '-') {
		usage();
		return -1;

	} else {
		char path[256];
		memset(path, 0, 256);
		strcpy(path, argv[1]);
		Class *mainClass = loadClassFromFile(path, argv[1]);
		if (NULL == mainClass) {
			fprintf(stderr, "Failed loadClass from file\n");
			return -1;
		}
		ClassEntry *clsEntry = CLASS_CE(mainClass);
		logClassEntry(clsEntry);

		//TOD
		// freeClass();

		return 0;
	}
}

static void usage() {
	printf("Usage\n");
	printf("  jvmp CLASS\n");
}
