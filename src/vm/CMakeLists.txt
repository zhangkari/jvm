SET(libjar_dir ../../libs)
SET(libvmbase_dir ../../include)
INCLUDE_DIRECTORIES(${libjar_dir} ${libvmbase_dir})

AUX_SOURCE_DIRECTORY(. src_files)
ADD_LIBRARY(libvmbase ${src_files})
TARGET_LINK_LIBRARIES(libvmbase libminiz -ldl)
