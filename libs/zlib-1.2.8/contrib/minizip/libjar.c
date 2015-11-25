/*****************************************
 * file name:		jar.c
 * description:		implement jar utility
 * date:			2015-10-08           
 * author: kari.zhang
 *
 ****************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "unzip.h"
#include "zip.h"
#include "libjar.h"

/*
 * Execute unpack jar file
 * Parameters:
 *		cb_progress can not be NULL
 */
void executeUnpackJar (
		const char* path,
		cb_unzip_start cb_start, 
		cb_unzip_progress cb_progress, 
		cb_unzip_error cb_error,
		cb_unzip_finish cb_finish)
{
	if (NULL == cb_progress) {
		if (NULL != cb_error) {
			cb_error (E_NULL_POINTER, 0);
		}

		return;
	}

	unzFile uzf = unzOpen(path);
	if (NULL == uzf) {
		if (NULL != cb_error) {
			cb_error (E_FILE_NOT_EXIST, 0);
		}

		return;
	}

	do {
		unz_global_info info;
		int ret = unzGetGlobalInfo(uzf, &info);
		if (ret != UNZ_OK) {
			break;
		}

		uLong num = info.number_entry;
		if (NULL != cb_start) {
			cb_start (num);
		}
		unzGoToFirstFile(uzf);

		unz_file_info finfo;
#define MAX_PATH 256
		char filename[MAX_PATH];
		int i;
		for (i = 0; i < num; ++i) {
			memset (filename, 0, MAX_PATH);
			unzGetCurrentFileInfo(uzf, &finfo, filename, MAX_PATH, NULL, 0, NULL, 0);
			unzOpenCurrentFile(uzf);

			char *mem = (char *)calloc (1, finfo.uncompressed_size);
			if (NULL == mem) {
				break;
			}

			unzReadCurrentFile (uzf, mem, finfo.uncompressed_size);
			cb_progress (i, filename, mem, finfo.uncompressed_size);
			unzCloseCurrentFile(uzf);
			unzGoToNextFile(uzf);
		}

		unzClose (uzf);
		if (NULL != cb_finish) {
			cb_finish();
		}

		return;

	} while (0);

	unzClose(uzf);
	if (NULL != cb_error) {
		cb_error (E_INTERNAL, 0);
	}
}

static int fileNotExist (const char* path) {
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


/*
 * Execute pack files into a jar file
 * Parameters:
 *		files:		input file list
 *		n:			file count
 *		destname:	output jar path
 * Return:
 *		 0 OK
 *		-1 EROR
 */
int executePackJar (const char **files, int n, const char *destname)
{
	if (NULL == files || NULL == destname) {
		return -1;
	}

	zipFile zfOut = zipOpen (destname, APPEND_STATUS_CREATE);
	if (NULL == zfOut) {
		printf ("Failed open %s for append\n", destname);
		return -1;
	}

	int i;
	for (i = 0; i < n; ++i) {
		if (fileNotExist (files[i])) {
			printf ("file %s not exist.\n", files[i]);
			continue;	
		}
		int retCode = zipOpenNewFileInZip (zfOut,
				files[i],
				NULL,		// zip_fileinfo
				NULL,		// extrafield_local
				0,			// size extrafield_local
				NULL,		// extrafield_global
				0,			// size extrafield_global
				NULL,		// comment
				Z_DEFLATED,
				Z_DEFAULT_COMPRESSION);

		if (retCode < 0) {
			printf ("add %s to %s failed.\n", files[i], destname); 
		}

		char *mem = NULL;
		int length = readFile (files[i], &mem);
		if (length >= 0) {
			retCode = zipWriteInFileInZip (zfOut, mem, length);
			free (mem);
		}
		zipCloseFileInZip (zfOut);
	}

	zipClose (zfOut, NULL);
	return 0;
}
