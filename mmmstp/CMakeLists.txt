cmake_minimum_required (VERSION 2.8.12.2)

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
								metaheuristic.h
								${includes})
target_link_libraries (genetic_mmmstp graph sttree sttalgo myutil)

#MESSAGE ("\tINFO: Compiling youhchen using network configurated for REAL=${REAL}")
#add_executable (yuhchen yuhchen.cpp yuhchen.h ${includes})
#target_link_libraries (yuhchen graph sttalgo sttree myutil)

MESSAGE ("\tINFO: Compiling Grasp Algorithm using network configurated for REAL=${REAL}")
add_executable (grasp grasp.cpp	
						grasp.h
						${includes})
target_link_libraries (grasp graph sttree sttalgo myutil)

MESSAGE ("\tINFO: Compiling Heuristic Algorithm based on Shortest Path")
add_executable (spheuristic sph_heuristic.cpp ${includes})
target_link_libraries (spheuristic graph sttalgo sttree myutil)

MESSAGE ("\tINFO: Compiling Heuristic Algorithm based on AGM")
add_executable (agmheuristic agm_heuristic.cpp ${includes})
target_link_libraries (agmheuristic graph sttalgo sttree myutil)

add_executable (tabu_test tabusearch.h tabusearch.cpp ${includes})
target_link_libraries (tabu_test graph sttree sttalgo myutil)

add_executable (minmax minmax.cpp minmax.h ${includes})
target_link_libraries (minmax graph sttree sttalgo myutil)

add_executable (minmax2 minmax2.cpp minmax.h ${includes})
target_link_libraries (minmax2 graph sttree sttalgo myutil)

add_executable (yuh yuh.cpp ${includes})
target_link_libraries (yuh graph sttree sttalgo myutil)

add_executable (mmx mmx.cpp ${includes})
target_link_libraries (mmx graph sttree sttalgo myutil)

add_executable (mcf mcf.cpp ${includes})
target_link_libraries (mcf graph sttree sttalgo myutil)

add_executable (spx spx.cpp ${includes})
target_link_libraries (spx graph sttree sttalgo myutil)

#add_executable (limite_agm limite.cpp limite.h ${includes})
#target_link_libraries (limite_agm graph sttalgo sttree)
