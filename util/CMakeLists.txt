cmake_minimum_required (VERSION 2.8.7)

SET(CPACK_SOURCE_IGNORE_FILES 
CMakeFiles;
CPack*;
cmake_install*;
CMakeCache*;
)

FILE(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE(GLOB HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

FILE (GLOB INCLUDE "${PROJECT_SOURCE_DIR}/include/*.h")

add_library (myutil STATIC rcatime.cpp)

add_executable (modelData DataGenerator.cpp SolverDataGenerator.h SolverDataGenerator.cpp ${INCLUDE})
target_link_libraries (modelData graph sttree)

add_executable (lpGenerator LpGenerator.cpp SolverDataGenerator.h SolverDataGenerator.cpp ${INCLUDE})
target_link_libraries (lpGenerator graph sttree)

add_executable (solutionTest SolverSolutionTest.cpp SolverSolutionTest.h ${INCLUDE})
target_link_libraries (solutionTest graph sttree)

INSTALL (FILES ${SRC} ${HDRS} CMakeLists.txt DESTINATION util)


