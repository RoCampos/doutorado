cmake_minimum_required (VERSION 2.8.12.2)

SET(CPACK_SOURCE_IGNORE_FILES 
CMakeFiles;
CPack*;
cmake_install*;
CMakeCache*;
)

add_subdirectory (Chen2000)
add_subdirectory (myalgorithm)

INSTALL (FILES CMakeLists.txt DESTINATION mpp_algorithms)
