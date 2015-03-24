/*******************************
 * file name:   buffer.h
 * description: wrap buffer
 * date:        2015-01-19
 * author:      kari.zhang
 *
 * modifications:
 *
 * ****************************/

#ifndef __BUFFER__H__
#define __BUFFER__H__

#include <comm.h>

typedef struct buffer_s
{
    void *data;
    uint32 len;
    uint32 size;
} buffer_s;

/**
 * init the buffer
 * params:
 *      buf:  buffer will be initialized
 * return:
 *      success 0
 *      failed  -1
 */
int initBuffer(buffer_s *buf); 
    
/**
 * new a buffer and initialized by data
 * params:
 *      data:  initialing value
 * return:
 *      buffer_s pointer
 *      NULL if failed
 */
buffer_s* newBuffer(int size);

/**
 * free the buffer
 */
void freeBuffer(buffer_s *buf);

/**
 * append data to the buffer
 * params:
 *      buf     buffer will be appended to
 *      data    the data will append
 * return:
 *      success 0
 *      failed < 0
 */
int appendBuffer(buffer_s *buf, const char *data);

/**
 * copy buffer
 * params:
 *      det:   the destination buffer
 *      src:   the source buffer
 * return:
 *      0   success
 *      -1  failed
 */
int copyBuffer(buffer_s* dst, buffer_s* src);

/**
 * ensure the buffer have engough capacity, if not, enlarge it.
 * params:
 *      buf:        the target buffer
 *      size:       the capacity to ensure
 * return:
 *      success: 0
 *      failed: -1 
 */
int ensureBuffer(buffer_s *buf, int size);

/**
 * clear the buffer, just set len = 0 
 * params:
 *      buf     buffer will be clear
 * return:
 *      success 0
 *      failed  < 0
 */
int clearBuffer(buffer_s *buf);

#endif
