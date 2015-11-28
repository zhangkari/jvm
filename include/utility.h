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

#ifdef DEBUG
#define LogD(...) printf(__VA_ARGS__)
#else
#define LogD(...)
#endif

#define LOG_FUNC_ENTRY LogD("++++ %s ++++\n", __func__) 
#define LOG_FUNC_EXIT LogD("---- %s ----\n", __func__) 

#endif
