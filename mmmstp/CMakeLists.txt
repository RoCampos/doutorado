cmake_minimum_required (VERSION 2.8.7)

SET(CPACK_SOURCE_IGNORE_FILES 
CMakeFiles;
CPack*;
cmake_install*;
CMakeCache*;
)

FILE(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE(GLOB HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
FILE(GLOB INCLUDE "${PROJECT_SOURCE_DIR}/include/*.h")

IF (REAL)
	add_definitions (-DMODEL_REAL)	
ELSE()
	add_definitions (-DMODEL_UNIT)
ENDIF()

MESSAGE ("\nINFO: On ${CMAKE_CURRENT_SOURCE_DIR} compile info")
INSTALL (FILES ${SRC} ${HDRS} CMakeLists.txt DESTINATION mmmstp)

MESSAGE ("\tINFO: Compiling genetic_mmmstp using network configurated for REAL=${REAL}")
add_executable (genetic_mmmstp 	genetic_algorithm.cpp 
								genetic_algorithm.h 
								metaheuristic.h)
target_link_libraries (genetic_mmmstp graph sttree sttalgo myutil)

