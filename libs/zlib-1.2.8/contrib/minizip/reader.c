/********************************************
 * file name:	reader.c
 * author:		kari.zhang
 * date:		
 *
 * modifications:
 *	1. Review code @ 2015-11-25 by kari.zhang
 *
 *******************************************/

#include <stdio.h>
#include "unzip.h"

int main(int argc, char** argv)
{
	if (2 != argc) {
		printf("Usage:  %s path\n", argv[0]);
		return -1;
	}
	
	unzFile uzf = unzOpen(argv[1]);
	if (NULL == uzf) {
		printf("Failed open %s\n", argv[1]);
		return -2;
	}

	do {
		unz_global_info info;
		int ret = unzGetGlobalInfo(uzf, &info);
		if (ret != UNZ_OK) {
			printf("Failed get global info\n");
			break;
		}
		
		uLong num = info.number_entry;
		printf("number entry = %ld\n", num);

		unzGoToFirstFile(uzf);

		unz_file_info finfo;
		char filename[64];
		int i;
		for (i = 0; i < num; ++i) {
			unzGetCurrentFileInfo(uzf, &finfo, filename, 64, NULL, 0, NULL, 0);
			printf("file name:%s\n", filename);
			printf("compressed size:%ld, uncompressed size:%ld\n\n", finfo.compressed_size, finfo.uncompressed_size);
			
			unzOpenCurrentFile(uzf);
			
			// unzReadCurrentFile(uzf, buff, 256);

			unzCloseCurrentFile(uzf);
			
			unzGoToNextFile(uzf);
		}

	} while (0);

	unzClose(uzf);

	return -3;
}
