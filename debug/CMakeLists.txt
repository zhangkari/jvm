PROJECT(jvm)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

# debug
SET(CMAKE_BUILD_TYPE DEBUG)
SET(CMAKE_C_FLAGS "-g -Wall")

# define -DDEBUG
ADD_DEFINITIONS(-DDEBUG)

SET(libs_dir ../libs)
SET(include_dir ../include)
SET(jvmtest_dir ../src/test)

# libminiz
SET(libminiz_dir ../libs/minizip)
ADD_SUBDIRECTORY(${libminiz_dir} ./libs)
INCLUDE_DIRECTORIES(${libs_dir} ${include_dir} ${jvmtest_dir})

# libvmbase
SET(libvmbase_dir ../src/vm)
ADD_SUBDIRECTORY(${libvmbase_dir} ./vm)

# libjvmtest
SET(libjvmtest_dir ../src/test)
ADD_SUBDIRECTORY(${libjvmtest_dir} ./jvmtest)

# project src files
AUX_SOURCE_DIRECTORY(../src src_files)

# source directory
SET(src_dir ../src)

# jvmtest
ADD_EXECUTABLE(testjvm ${src_dir}/jvmtest.c)
TARGET_LINK_LIBRARIES(testjvm libminiz libvmbase libjvmtest cunit)

# jvm
ADD_EXECUTABLE(jvm ${src_dir}/jvm.c)
TARGET_LINK_LIBRARIES(jvm libminiz libvmbase)

# jvmp
ADD_EXECUTABLE(jvmp ${src_dir}/jvmp.c)
TARGET_LINK_LIBRARIES(jvmp libvmbase)

# jar
ADD_EXECUTABLE(jar ${src_dir}/jar.c)
TARGET_LINK_LIBRARIES(jar libminiz libvmbase)

# jvmd
ADD_EXECUTABLE(jvmd ${src_dir}/jvmd.c)
TARGET_LINK_LIBRARIES(jvmd libminiz libvmbase)
