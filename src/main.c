/*******************************
 * file name:   main.c
 * description: program entry 
 * date:        2015-01-19
 * author:      kari.zhang
 *
 * modifications:
 *
 * 1. Modified by Kari.Zhang @ 2015-01-19
 *      rearchitect & redesign
 * ****************************/

#include <ClassFile.h>
#include <stdio.h>
#include <instruction.h>

int main(int argc, char *argv[]) {
    if (2 != argc) {
        printf("%s [options] class\n", argv[0]);
        return -1;
    }

    ClassFile *clsFile = loadClassFile(argv[1]);
    if (NULL == clsFile) {
        return -1;
    }
	logClassFile(clsFile);
    freeClassFile(clsFile);
    clsFile = NULL;
}
