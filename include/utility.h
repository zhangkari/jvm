/*******************************
 * file name:   utility.h
 * description: define utilities  
 * date:        2015-11-29
 * author:      kari.zhang
 *
 * modifications:
 *
 * ****************************/

#ifndef __UTILITY__H__
#define __UTILITY__H__

#define D_TAG "DEBUG:"
#ifdef DEBUG
#define LogD(format, ...) printf(D_TAG format " %s:%d\n", ##__VA_ARGS__, __FILE__, __LINE__)
#else
#define LogD(...)
#endif

#define LOG_FUNC_ENTRY LogD("++++ %s ++++\n", __func__) 
#define LOG_FUNC_EXIT LogD("---- %s ----\n", __func__) 

#endif
