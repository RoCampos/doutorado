cmake_minimum_required(VERSION 2.8.12.2)

SET(CPACK_SOURCE_IGNORE_FILES 
CMakeFiles;
CPack*;
cmake_install*;
CMakeCache*;
)

project (algorithm)

set (PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set (PROJECT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set (HEURISTIC_DIR ${PROJECT_SOURCE_DIR}/heuristics)

add_subdirectory (heuristics)
#add_subdirectory (transgenetic)

INSTALL (FILES CMakeLists.txt DESTINATION algorithm)

