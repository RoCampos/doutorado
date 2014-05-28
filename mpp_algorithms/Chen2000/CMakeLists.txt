cmake_minimum_required (VERSION 2.8.7)

SET(CPACK_SOURCE_IGNORE_FILES 
CMakeFiles;
CPack*;
cmake_install*;
CMakeCache*;
)

#enable_testing ();

FILE (GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE (GLOB includes "${PROJECT_SOURCE_DIR}/include/*.h")
FILE (GLOB HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable (lowerbound lowerbound.cpp ${includes})
target_link_libraries (lowerbound graph)

add_executable (datagen datagenerator.cpp ${includes})
target_link_libraries (datagen graph)

add_executable (filter_sol filter.cpp ${includes})
target_link_libraries (filter_sol graph)

add_executable (chentest chentest.cpp chenalgorithm.h chenalgorithm.cpp ${includes})
target_link_libraries (chentest graph)

#adicionando teste
#add_test (inicializacao )

