cmake_minimum_required (VERSION 2.8.12.2)

SET(CPACK_SOURCE_IGNORE_FILES 
CMakeFiles;
CPack*;
cmake_install*;
CMakeCache*;
)

FILE(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE(GLOB HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library (sttalgo STATIC ${SRC})
target_link_libraries (sttalgo sttree graph)

INSTALL (FILES ${SRC} ${HDRS} CMakeLists.txt DESTINATION stalgorithm)

#install (TARGETS sttree
#	DESTINATION lib
#	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
