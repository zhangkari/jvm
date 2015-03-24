/*******************************
 * file name:   buffer.c
 * description: utils for buffer 
 * date:        2015-01-19
 * author:      kari.zhang
 *
 * modifications:
 *
 * ****************************/

#include <stdlib.h>
#include <string.h>
#include <buffer.h>

/**
 * new a buffer and initialized by data
 * params:
 *      data:  initialing value
 * return:
 *      buffer_s pointer
 *      NULL if failed
 */
buffer_s* newBuffer(int size)
{
    if (size < 1) {
        return NULL;
    }

    buffer_s *buf = (buffer_s*)calloc(sizeof(buffer_s), 1);
    if (NULL == buf) {
        return NULL;
    }

    buf->size = size;
    buf->len = 0;

    buf->data = (char *)calloc(buf->size, 1);
    if (NULL == buf->data) {
        free(buf);
        buf = NULL;
        return buf;
    }

    return buf;
}

/**
 * init the buffer
 * params:
 *      buf:  buffer will be initialized
 * return:
 *      success 0
 *      failed  -1
 */
int initBuffer(buffer_s *buf) 
{
    VALIDATE_NOT_NULL(buf);

    buf->data = calloc(4, 1);
    buf->len  = 0;
    buf->size = 4;

    return 0;
}

/**
 * free the buffer
 */
void freeBuffer(buffer_s *buf)
{
    if (NULL != buf) {
        if (NULL != buf->data) {
            free(buf->data);
            buf->data = NULL;
        } 
        free(buf);
    }
}

/**
 * ensure the buffer have engough capacity, if not, enlarge it.
 * params:
 *      buf:        the target buffer
 *      size:       the capacity to ensure
 * return:
 *      success: 0
 *      failed: -1 
 */
int ensureBuffer(buffer_s *buf, int size)
{
    VALIDATE_NOT_NULL(buf);

    if (buf->size - buf->len >= size) {
        return 0;
    } 

    void *ptrNew = realloc(buf->data, size + buf->len);
    if (NULL == ptrNew) {
        LogE("Failed malloc for enlarge\n");
        return -1;
    } 

    buf->data = ptrNew;
    buf->size = size + buf->len;

    return 0;
}


/**
 * append data to the buffer
 * params:
 *      buf     buffer will be appended to
 *      data    the data will append
 * return:
 *      success 0
 *      failed < 0
 */
int appendBuffer(buffer_s *buf, const char *data)
{
    VALIDATE_NOT_NULL2(buf, data);

    int len = strlen(data);
    if (ensureBuffer(buf, buf->len + len) < 0) {
        LogE("Failed malloc for append\n");
        return -1;
    } 

    strncpy(buf->data + buf->len, data, len);
    buf->size = buf->len + len;
    buf->len = buf->len + len;

    return 0;
}

/**
 * copy buffer
 * params:
 *      det:   the destination buffer
 *      src:   the source buffer
 * return:
 *      0   success
 *      -1  failed
 */
int copyBuffer(buffer_s* dst, buffer_s* src)
{
    VALIDATE_NOT_NULL2(dst, src);
    if(ensureBuffer(dst, src->len) < 0) {
        return -1;
    }

    dst->len = 0;
    dst->size = src->len;
    memmove(dst->data, src->data, src->len);

    return 0;
}

/**
 * clear the buffer, just set len = 0 
 * params:
 *      buf     buffer will be clear
 * return:
 *      success 0
 *      failed  < 0
 */
int clearBuffer(buffer_s *buf)
{
    VALIDATE_NOT_NULL(buf);

    buf->len = 0;
    return 0;
}
