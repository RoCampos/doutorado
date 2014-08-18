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

add_executable (mppalgorithm 
				main.cpp 
				mppalgorithm.cpp 				
				mppalgorithm.h
				breadth_search_tree.h				
				${includes})
target_link_libraries (mppalgorithm graph sttree)

add_executable (test_properties teste_properties.cpp 
		properties.h properties.cpp
		strategy_make_tree.cpp strategy_make_tree.h
		breadth_search_tree.h breadth_search_tree.cpp
		${includes})
target_link_libraries (test_properties graph sttree ${Boost_LIBRARIES})

