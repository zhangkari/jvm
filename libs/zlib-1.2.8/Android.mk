LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_SRC_FILES :=$(notdir $(filter-out %/example.c, $(wildcard $(LOCAL_PATH)/*.c)))


LOCAL_MODULE := libz
#include $(BUILD_SHARED_LIBRARY)
include $(BUILD_STATIC_LIBRARY)
