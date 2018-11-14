PROJECT(jvm)
CMAKE_MINIMUM_REQUIRED (VERSION 2.6)

# release
SET(CMAKE_BUILD_TYPE RELEASE)
SET(CMAKE_C_FLAGS "-O2")

SET(libs_dir ../libs)
SET(include_dir ../include)

# libminiz
SET(libminiz_dir ../libs/minizip)
ADD_SUBDIRECTORY(${libminiz_dir} ./libs)
INCLUDE_DIRECTORIES(${libs_dir} ${include_dir})

# libvmbase
SET(libvmbase_dir ../src/vm)
ADD_SUBDIRECTORY(${libvmbase_dir} ./vm)

# project src files
AUX_SOURCE_DIRECTORY(../src src_files)

# source directory
SET(src_dir ../src)

# jvm
ADD_EXECUTABLE(jvm ${src_dir}/jvm.c)
#TARGET_LINK_LIBRARIES(jvm libminiz)
TARGET_LINK_LIBRARIES(jvm libvmbase)

# jvmp
ADD_EXECUTABLE(jvmp ${src_dir}/jvmp.c)
TARGET_LINK_LIBRARIES(jvmp libvmbase)

# jarx
ADD_EXECUTABLE(jarx ${src_dir}/jar.c)
#TARGET_LINK_LIBRARIES(jarx libminiz)
TARGET_LINK_LIBRARIES(jarx libvmbase)

# jvmd
ADD_EXECUTABLE(jvmd ${src_dir}/jvmd.c)
TARGET_LINK_LIBRARIES(jvmd libminiz)
TARGET_LINK_LIBRARIES(jvmd libvmbase)
