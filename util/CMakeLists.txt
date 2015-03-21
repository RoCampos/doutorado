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

#SET (REAL OFF)

#used to read the values of instances as they are
IF (REAL)

	MESSAGE ( "-- modelData: COMPILATION FOR REAL VALUES OF INSTANCES(TK E CAPACITY)")
	add_definitions (-DMODEL_REAL)
	add_executable (realModelData DataGenerator.cpp SolverDataGenerator.h SolverDataGenerator.cpp ${INCLUDE})
	target_link_libraries (realModelData graph sttree)

ELSE()
#the values of capacity is equal the size of groups
#the values of tk(traffic request) for each group is equal 1
	MESSAGE ( "-- modelData: COMPILATION FOR VALUES OF INSTANCES(TK=1 E CAPACITY=GROUP SIZE)")
	add_definitions (-DMODEL_UNIT)
	add_executable (unitModelData DataGenerator.cpp SolverDataGenerator.h SolverDataGenerator.cpp ${INCLUDE})
	target_link_libraries (unitModelData graph sttree)
ENDIF()



add_executable (lpGenerator LpGenerator.cpp SolverDataGenerator.h SolverDataGenerator.cpp ${INCLUDE})
target_link_libraries (lpGenerator graph sttree)

add_executable (solutionTest SolverSolutionTest.cpp SolverSolutionTest.h ${INCLUDE})
target_link_libraries (solutionTest graph sttree)

INSTALL (FILES ${SRC} ${HDRS} CMakeLists.txt DESTINATION util)


