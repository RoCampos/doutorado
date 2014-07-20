cmake_minimum_required (VERSION 2.8.7)

SET(CPACK_SOURCE_IGNORE_FILES 
CMakeFiles;
CPack*;
cmake_install*;
CMakeCache*;
)

FILE(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE(GLOB HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library (sttree STATIC ${SRC})

add_executable (steinertest steiner_test.cpp)

INSTALL (FILES ${SRC} ${HDRS} CMakeLists.txt DESTINATION sttree)

#install (TARGETS sttree
#	DESTINATION lib
#	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
