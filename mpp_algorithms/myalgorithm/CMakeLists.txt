cmake_minimum_required (VERSION 2.8.7)

SET(CPACK_SOURCE_IGNORE_FILES 
CMakeFiles;
CPack*;
cmake_install*;
CMakeCache*;
)

FILE (GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE (GLOB includes "${PROJECT_SOURCE_DIR}/include/*.h")
FILE (GLOB HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_executable (mppalgorithm mppalgorithm.cpp main.cpp mppalgorithm.h ${includes})
target_link_libraries (mppalgorithm graph)

add_executable (main_strategy main_strategy.cpp 
		strategy_make_tree.cpp strategy_make_tree.h
		naiveshortestpath.h naiveshortestpath.cpp
		properties.cpp
		${includes})
target_link_libraries (main_strategy sttree graph)
		
add_executable (steinertest steiner_test.cpp ${includes})
target_link_libraries (steinertest graph sttree)

add_executable (test_properties teste_properties.cpp 
		properties.h properties.cpp
		${includes})
target_link_libraries (test_properties graph)

