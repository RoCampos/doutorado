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


include_directories (${GUROBI_INCLUDE_DIRS})
FILE(GLOB GRB "${GUROBI_INCLUDE_DIRS}/*.h")

add_executable (paretogen pareto.cpp models.h models.cpp 				
				${INCLUDE} 
				${GRB})
target_link_libraries (paretogen graph
						-lboost_regex						
						${GUROBI_CXX_LIBRARY} 
						${GUROBI_LIBRARIES})


add_executable (mathmodel mathmodel.cpp models.h models.cpp
				${INCLUDE}
				${GRB})

target_link_libraries (mathmodel graph
						-lboost_regex						
						${GUROBI_CXX_LIBRARY} 
						${GUROBI_LIBRARIES})

add_executable (steinermodel steinermodel.cpp models.h models.cpp
				${INCLUDE}
				${GRB})

target_link_libraries (steinermodel graph
						-lboost_regex
						${GUROBI_CXX_LIBRARY} 
						${GUROBI_LIBRARIES})


INSTALL (FILES ${SRC} ${HDRS} CMakeLists.txt DESTINATION multi)
