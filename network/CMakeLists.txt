cmake_minimum_required (VERSION 2.8.12.2)

SET(CPACK_SOURCE_IGNORE_FILES 
CMakeFiles;
CPack*;
cmake_install*;
CMakeCache*;
)

IF (REAL)
	add_definitions (-DMODEL_REAL)	
	MESSAGE ("\nCompiling NETWORK using REAL Values\n")
ELSE()
	add_definitions (-DMODEL_UNIT)
	MESSAGE ("\nCompiling NETWORK using UNIT Values\n")
ENDIF()

FILE(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
FILE(GLOB HDRS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library (graph STATIC ${SRC})

#add_executable (Kpathtest main.cpp)
#target_link_libraries (Kpathtest graph sttree heuristic)

INSTALL (FILES ${SRC} ${HDRS} CMakeLists.txt DESTINATION network)

#install (TARGETS graph
#	DESTINATION lib
#	PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)
