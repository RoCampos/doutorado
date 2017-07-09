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

	MESSAGE ("\nCompiling MATHMODEL using REAL Values\n")

ELSE()
	add_definitions (-DMODEL_UNIT)

	MESSAGE ("\nCompiling MATHMODEL using UNIT Values\n")
ENDIF()

include_directories (${GUROBI_INCLUDE_DIRS})
FILE(GLOB GRB "${GUROBI_INCLUDE_DIRS}/*.h")

MESSAGE ("-------------------- ${Boost_LIBRARIES}")

#add_executable (paretogen pareto.cpp models.h models.cpp
#				${Boost_INCLUDE_DIR}
#				${INCLUDE} 
#				${GRB})
#target_link_libraries (paretogen graph sttree
#						${Boost_LIBRARIES}						
#						${GUROBI_CXX_LIBRARY} 
#						${GUROBI_LIBRARIES})

#add_executable (mathmodel mathmodel.cpp models.h models.cpp
#				${Boost_INCLUDE_DIR}
#				${INCLUDE}
#				${GRB})
#target_link_libraries (mathmodel graph
#						${Boost_LIBRARIES}
#						${GUROBI_CXX_LIBRARY} 
#						${GUROBI_LIBRARIES})

#add_executable (steinermodel steinermodel.cpp models.h models.cpp
#				${Boost_INCLUDE_DIR}
#				${INCLUDE}
#				${GRB})
#target_link_libraries (steinermodel graph
#						${Boost_LIBRARIES}
#						${GUROBI_CXX_LIBRARY} 
#						${GUROBI_LIBRARIES})


add_executable (multitest main.cpp individual.h ${INCLUDE})
target_link_libraries (multitest graph sttree sttalgo)

add_executable (mainmulti mainmulti.cpp 
				individual.h 
				operator.h				
				generic_individual.h	
				problem_info.h
				spea2.h
				${INCLUDE})
target_link_libraries (mainmulti graph sttree sttalgo)


# compilation of parallel example of algorithms
# add_executable (parallel parallel.cpp ${INCLUDE})
#target_link_libraries (parallel graph sttree sttalgo myutil -fopenmp)


INSTALL (FILES ${SRC} ${HDRS} CMakeLists.txt DESTINATION multi)


