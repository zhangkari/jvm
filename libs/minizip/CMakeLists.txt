# Build libminizip

if (WIN32)
    MESSAGE(STATUS "Win32")
    SET(iowin32_src iowin32.c)
elseif (linux)
    MESSAGE(STATUS "Linux platform")
elseif (UNIX)
    MESSAGE(STATUS "Unix platorm")
endif()

SET(src_files ioapi.c mztools.c unzip.c zip.c libjar.c ${iowin32_src})

# include ../libjar.h
INCLUDE_DIRECTORIES(../)

ADD_LIBRARY(libminiz ${src_files})
TARGET_LINK_LIBRARIES(libminiz z)
