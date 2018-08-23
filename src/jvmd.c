/********************************************
 * file name:   jvmd.c
 * description: java virtual machine debugger
 * author:      kari.zhang
 *
 * modifications:
 *
 * 1. Created by Kari.Zhang @ 2016-01-11
 * *****************************************/

#include <stdio.h>
#include <string.h>

#define JVMD_VER "0.0.1"

static void logDebugger();
static void usage();

int main(int argc, char *argv[]) {

    if (2 != argc) {
        usage();
        return -1;
    }

    if (0 == strncmp(argv[1], "-v", 2)) {
        logDebugger();
        return 0;
    } else if (*argv[1] == '-') {
        usage();
        return -1;
    } else {
        printf("Not implemented yet!\n");
        return 0;
    }
}

static void logDebugger() {
    printf("jvmd %s compiled on %s copyright@kari.zhang\n", JVMD_VER, __DATE__);
}

static void usage() {
    printf("Usage:\n");
    printf("    jvmd CLASS_NAME\n");
}
