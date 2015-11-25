/********************************************
 * file name:	writer.c
 * author:		kari.zhang
 * date:
 *
 * modifications:
 *	1. Review code @ 2015-11-25 by kari.zhang
 *
 ********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <utime.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "zip.h"

int fileNotExist (const char* path) {
	int retCode = 0;
	FILE *fp = fopen (path, "rb");
	if (NULL == fp) {
		retCode = 1;
	}
	else {
		fclose (fp);
	}

	return retCode;
}

static int readFile (const char* path, char** mem) {
	FILE *fp = fopen (path, "rb");
	if (NULL == fp)  {
		return -1;
	}

	fseek (fp, 0, SEEK_END);
	long size = ftell (fp);
	fseek (fp, 0, SEEK_SET);

	*mem = (char *) calloc (1, size);
	if (NULL == *mem) {
		fclose (fp);
		return -2;
	}

	if (fread(*mem, size, 1, fp) != 1) {
		printf ("read %s error\n", path);
		free (*mem);
		fclose (fp);
		return -1;
	}

	fclose (fp);
	return size;
}

int main(int argc, char** argv)
{
	if (argc <= 2) {
		printf ("Usage:\n");
		printf ("    %s input1 input2 ... output\n", argv[0]);
		return -1;
	}

	zipFile zfOut = zipOpen (argv[argc-1], APPEND_STATUS_CREATE);
	if (NULL == zfOut) {
		printf ("Failed open %s for append\n", argv[argc-1]);
		return -1;
	}

	int i;
	for (i = 1; i < argc - 1; ++i) {
		if (fileNotExist (argv[i])) {
			printf ("file %s not exist.\n", argv[i]);
			continue;	
		}
		int retCode = zipOpenNewFileInZip (zfOut,
				argv[i],
				NULL,		// zip_fileinfo
				NULL,		// extrafield_local
				0,			// size extrafield_local
				NULL,		// extrafield_global
				0,			// size extrafield_global
				NULL,		// comment
				Z_DEFLATED,
				Z_DEFAULT_COMPRESSION);

		if (retCode < 0) {
			printf ("add %s to %s failed.\n", argv[i], argv[argc-1]); 
		}

		char *mem = NULL;
		int length = readFile (argv[i], &mem);
		if (length >= 0) {
			retCode = zipWriteInFileInZip (zfOut, mem, length);
			free (mem);
		}
		zipCloseFileInZip (zfOut);
	}

	zipClose (zfOut, NULL);
}
