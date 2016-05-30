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
 *
 * 2. Move other functions to runtime.c,
 *		leave main() only.
 *		by kari.zhang@2016-5-30
 ******************************/

#include <stdio.h>
#include <string.h>
#include "class.h"
#include "runtime.h"

int main(int argc, char *argv[]) {
	InitArgs initArgs;	
	setDefaultInitArgs(&initArgs);

	Property *props = NULL;
	int len = parseCmdLine(argc, argv, &props);
	setInitArgs(props, len, &initArgs);
	
    VM vm;
	memset(&vm, 0, sizeof(vm));
	initVM(&initArgs, &vm);

    char path[256];
    strcpy(path, argv[1]);
    strcat(path, ".class");
	Class *mainClass = loadClassFromFile(path, argv[1]);
    if (NULL == mainClass) {
        fprintf(stderr, "Failed loadClass from file\n");
        return -1;
    }
	initArgs.mainClass = mainClass;

    linkClass(mainClass);
	startVM(&vm);
	destroyVM(&vm);

	return 0;
}
