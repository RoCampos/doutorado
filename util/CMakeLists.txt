cmake_minimum_required (VERSION 2.8.7)

SET(CPACK_SOURCE_IGNORE_FILES 
CMakeFiles;
CPack*;
cmake_install*;
CMakeCache*;
)

FILE(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cc")
FILE(GLOB HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library (myutil STATIC ${SRC})

INSTALL (FILES ${SRC} ${HDRS} CMakeLists.txt DESTINATION myutil)

add_executable (teste mystringtest.cpp ${includes})
target_link_libraries (teste myutil)
