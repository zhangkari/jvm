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

static void usage() {
	printf("Usage\n");
	printf("  %s [-c f1 f2 ...] [-d] path \n", "jar");
	printf("       -c: compress files into a jar\n");
	printf("       -d: decompress files from a jar\n");
}

static void on_start (int total) {
	printf("total %d files\n", total);
}

static void on_progress (int index, 
		const char* name,
		const char* mem,
		int size) {

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

static void on_finish() {
	printf ("extract complete.\n");
}

int main(int argc, char *argv[]) {
	if (argc <= 2) {
		usage();	
		return -1;
	}

	if (strcmp(argv[1], "-d") == 0) {
		if (argc != 3) {
			usage();
			return -1;
		}

		executeUnpackJar (argv[2], on_start, on_progress, NULL, on_finish);
	}
	else if (strcmp(argv[1], "-c") == 0) {
		if (argc <= 3) {
			usage();
			return -1;
		}
		else {
			executePackJar ((const char**)&argv[2], argc-3, argv[argc-1]);	
		}
	}
	else {
		usage();
		return -1;
	}
}
