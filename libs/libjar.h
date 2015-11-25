/**************************
 file nae: jar.h
 description: jar utility
 date: 2015-10-08           
 author: kari.zhang
 *************************/

#ifndef __JAR__H__
#define __JAR__H__

#define OK					 0
#define E_FILE_NOT_EXIST	-1
#define E_NULL_POINTER		-2
#define E_ALLOC_MEM			-3
#define E_INTERNAL			-4

/**
 * will be invoked when start unzip
 */
typedef void (* cb_unzip_start) (int total);

/*
 * will be invoked when unzip in progress
 */
typedef void (* cb_unzip_progress) (
		int index, 
		const char* name,
		const char* mem,
		int size);

/*
 * will be invoked when unzip error occurs
 * Return:
 *		0  continue to unzip
 *		1  stop unzip
 */
typedef int (* cb_unzip_error) (int errcode, int index);

/*
 * will be invoked when unzip finish
 */
typedef void (* cb_unzip_finish) ();

/*
 * Execute unpack jar file
 * Parameters:
 *		cb_progress can not be NULL
 */
void executeUnpackJar (
		const char			*path,
		cb_unzip_start		cb_start, 
		cb_unzip_progress	cb_progress, 
		cb_unzip_error		cb_error,
		cb_unzip_finish		cb_finish);

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
int executePackJar (const char **files, int n, const char *destname);

#endif
