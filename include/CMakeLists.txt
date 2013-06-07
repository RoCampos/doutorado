cmake_minimum_required (VERSION 2.8.7)

FILE(GLOB files "${CMAKE_CURRENT_SOURCE_DIR}/.h")

SET(CPACK_SOURCE_IGNORE_FILES
CMakeFiles;
cmake_install*;
CMakeCache*;
install*;
*~;
) 

#install (FILES ${files}
#	DESTINATION include
#	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
