/*******************************
 * file name:   jar.c
 * description: jar utility
 * date:        2015-11-25
 * author:      kari.zhang
 *
 * modifications:
 *
 * 1. Created by Kari.Zhang @ 2015-01-19
 * ****************************/

#include <stdio.h>
#include <string.h>
#include "libjar.h"

#define JAR_VER "0.0.1"

static void usage() {
    printf("Usage\n");
    printf("  %s [-c f1 f2 ...] [-d] path \n", "jar");
    printf("       -c: compress files into a jar\n");
    printf("       -d: decompress files from a jar\n");
}

/*
 * Unpack jar start
 */
static void on_start (int total, void* param) {
    printf("total %d files\n", total);
}

/*
 * Unpack jar progress
 */
static void on_progress (int index, 
        const char* name,
        const char* mem,
        int size,
        void* param) {

    if (NULL == name || mem == NULL) {
        printf ("name or mem is NULL\n");
        return;
    }

    FILE *fp = fopen(name, "wb");
    if (NULL == fp) {
        printf ("failed create %s\n", name);
        return;
    }

    if (fwrite (mem, size, 1, fp) != 1) {
        printf ("failed write %s\n", name);
    }

    fclose (fp);
    printf ("extract %s complete\n", name);
}

/*
 * Unpack jar finish
 */
static void on_finish(void *param) {
    printf ("extract complete.\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        usage();	
        return -1;
    }

    if (2 == argc) {
        if (0 == strncmp("-v", argv[1], 2)) {
            printf("jar %s compiled on %s copyright@kari.zhang.\n", JAR_VER, __DATE__);
            return 0;
        } else {
            usage();
            return -1;
        }
    }

    if (strcmp(argv[1], "-d") == 0) {
        if (argc != 3) {
            usage();
            return -1;
        }

        executeUnpackJar (argv[2], 
                on_start,
                on_progress,
                NULL, 
                on_finish,
                NULL);

        return 0;

    } else if (strcmp(argv[1], "-c") == 0) {
        if (argc <= 3) {
            usage();
            return -1;
        }

        executePackJar ((const char**)&argv[2], argc-3, argv[argc-1]);	

        return 0;

    } else {
        usage();
        return -1;
    }
}
