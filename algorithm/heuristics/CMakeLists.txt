cmake_minimum_required (VERSION 2.8.12.2)

SET(CPACK_SOURCE_IGNORE_FILES 
CMakeFiles;
CPack*;
cmake_install*;
CMakeCache*;
)

FILE (GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE (GLOB includes "${PROJECT_SOURCE_DIR}/include/*.h")

add_library (heuristic STATIC ${SRCS} ${includes})
target_link_libraries (heuristic graph sttree)

add_executable (preProcessingPaths main.cpp)
target_link_libraries (preProcessingPaths heuristic graph sttree)

#add_executable (readPathsTest infoReaderTest.cpp)
#target_link_libraries (readPathsTest heuristic graph sttree)

#install (TARGETS heuristic
 #       DESTINATION lib
  #      PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

FILE (GLOB HDRS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
INSTALL (FILES ${SRCS} ${HDRS} CMakeLists.txt DESTINATION algorithm/heuristics)
