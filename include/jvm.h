/*******************************
 * file name:   jvm.h
 * description: define jvm 
 * date:        2015-01-19
 * author:      kari.zhang
 *
 * modifications:
 *
 * ****************************/

#ifndef __JVM__H__
#define __JVM__H__

#define MIN_HEAP 4*KB
#define MIN_STACK 1*KB

#define DEFAULT_MIN_HEAP 16*MB
#define DEFAULT_MAX_HEAP 128*MB
#define DEFAULT_STACK 256*KB

/*
 * size of emergency area - big enough to create
 * a StackOverFlow exception 
 */
#define STACK_RED_ZONE_SIZE 1*KB

#define JVM_VERSION "0.0.1" 

#endif
