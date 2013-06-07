cmake_minimum_required (VERSION 2.8.7)

SET(CPACK_SOURCE_IGNORE_FILES
CMakeFiles;
CPack*;
cmake_install*;
CMakeCache*;
)

FILE(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE(GLOB HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library (statistical STATIC ${SRC})

add_executable (leitor main.cpp)
target_link_libraries (leitor statistical)

add_executable (media mean.cpp)
target_link_libraries (media statistical)

INSTALL (FILES ${SRC} ${HDRS} CMakeLists.txt DESTINATION statistics)

#install (TARGETS graph
#       DESTINATION lib
#       PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)

