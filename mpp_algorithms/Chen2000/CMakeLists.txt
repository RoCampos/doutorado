cmake_minimum_required (VERSION 2.8.12.2)

SET(CPACK_SOURCE_IGNORE_FILES 
CMakeFiles;
CPack*;
cmake_install*;
CMakeCache*;
)

FILE (GLOB SRCS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE (GLOB includes "${PROJECT_SOURCE_DIR}/include/*.h")
FILE (GLOB HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

INSTALL (FILES ${SRCS} ${HDRS} CMakeLists.txt DESTINATION mpp_algorithms/Chen2000)

add_executable (lowerbound lowerbound.cpp ${includes})
target_link_libraries (lowerbound graph)

add_executable (datagen datagenerator.cpp ${includes})
target_link_libraries (datagen graph)

add_executable (filter_sol filter.cpp ${includes})
target_link_libraries (filter_sol graph)

add_executable (chentest chentest.cpp chenalgorithm.h chenalgorithm.cpp ${includes})
target_link_libraries (chentest graph)

add_executable (parser parser.cpp parser.h)
target_link_libraries (parser -lboost_regex)

add_executable (optimalmpp optimalmpp.cpp)
target_link_libraries (optimalmpp graph)

add_executable (kmbopt stalgo.cpp ${includes})
target_link_libraries (kmbopt graph sttree sttalgo)