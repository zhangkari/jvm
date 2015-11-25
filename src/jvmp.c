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

static void usage();

int main(int argc, char *argv[]) {
	if (2 != argc) {
		usage();
		return -1;
	}
    char path[256];
    strcpy(path, argv[1]);
    strcat(path, ".class");
    Class *mainClass = loadClassFromFile(path, argv[1]);
    if (NULL == mainClass) {
        fprintf(stderr, "Failed loadClass from file\n");
        return -1;
    }
    ClassEntry *clsEntry = CLASS_CE(mainClass);
    logClassEntry(clsEntry);

    //TOD
    // freeClass();
}

static void usage() {
	printf("Usage\n");
	printf("  jvmp CLASS\n");
}
