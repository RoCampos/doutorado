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

INSTALL ( FILES ${SRCS} ${HDRS} CMakeLists.txt DESTINATION mpp_algorithms/myalgorithm)

set (BST OFF)
set (BSTCOST OFF)
set (TESTALGO OFF)
set (BMEAN OFF)
set (ACOMPP_CAP ON)

#define globally for all modules. See the Project Root directory;
IF (REAL)
	add_definitions ( -DMODEL_REAL)
ELSE ()
	add_definitions ( -MODEL_UNIT)
ENDIF()

#COMPILATION DEFINITION FO BSTREE
IF (BST) 
add_definitions ( -DBSTTREE )
add_executable (bstree 
				main.cpp 
				mppalgorithm.cpp 				
				mppalgorithm.h
				breadth_search_tree.h
				breadth_search_tree.cpp
				${includes})
target_link_libraries (bstree graph sttree myutil)
ENDIF()

#COMPILATION DEFINITION FOR BST_COST ALGORITHM
IF (BSTCOST)
add_definitions ( -DBSTCOST )
add_executable (bscost 
				main.cpp 
				mppalgorithm.cpp
				mppalgorithm.h
				bstcost.h
				bstcost.cpp
				${includes})
target_link_libraries (bscost graph sttree myutil)
ENDIF ()

IF (BMEAN)
add_definitions ( -DBSTMEAN )
add_executable (bsmean 
				main.cpp 
				mppalgorithm.cpp
				mppalgorithm.h
				bstmean.h
				bstmean.cpp
				${includes})
target_link_libraries (bsmean graph sttree myutil)
ENDIF()

#IF ACOMPP_CAP = ON  então compila para capacidade residual
IF (ACOMPP_CAP)
MESSAGE ("-- ACOMPP: RES_CAP=ON - COMPILATION FOR RESDIUAL CAPACITY")
add_definitions (-DRES_CAP)
ELSE()
MESSAGE ("-- ACOMPP: RES_CAP=OFF - COMPILATION FOR CONGESTION")
add_definitions (-DCONG)
ENDIF ()
add_executable (acompp acomppmain.cpp 
		aco.h aco.cpp 
		acompp.h acompp.cpp
		ant.h ant.cpp
		edgecontainer.h edgecontainer.cpp
		${includes})
target_link_libraries (acompp graph sttree myutil ${Boost_LIBRARIES})

IF (TESTALGO)
add_executable (test_properties teste_properties.cpp 
		aco.h aco.cpp 
		acompp.h acompp.cpp
		ant.h ant.cpp
		edgecontainer.h edgecontainer.cpp
		${includes})
target_link_libraries (test_properties graph sttree ${Boost_LIBRARIES})
ENDIF ()
