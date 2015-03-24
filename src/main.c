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

#include <stdio.h>
#include <stdlib.h>
#include <classloader.h>

int main(int argc, char *argv[]) 
{
    if (2 != argc) {
        printf("%s [options] class\n", argv[0]);
        return -1;
    }

    ClsLoader_t *loader = load_class(argv[1]);
    if (NULL == loader) {
        return -1;
    }

    free(loader);
    loader = NULL;
}
