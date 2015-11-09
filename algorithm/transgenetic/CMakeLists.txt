cmake_minimum_required (VERSION 2.8.12.2)

SET(CPACK_SOURCE_IGNORE_FILES 
CMakeFiles;
CPack*;
cmake_install*;
CMakeCache*;
)

FILE (GLOB HEU_SRC heuristic/*.h)

add_executable (transgenetic1 transgenetic.cpp version1.cpp ${HEU_SRC})
target_link_libraries (transgenetic1 graph sttree heuristic)

add_executable (transgenetic2 transgenetic.cpp version2.cpp ${HEUR_SRC})
target_link_libraries (transgenetic2 graph sttree heuristic)

add_executable (transgenetic3 transgenetic.cpp version3.cpp ${HEUR_SRC})
target_link_libraries (transgenetic3 graph sttree heuristic)

add_executable (transgenetic4 transgenetic.cpp version4.cpp ${HEUR_SRC})
target_link_libraries (transgenetic4 graph sttree heuristic)

add_executable (transgenetic5 transgenetic.cpp version5.cpp ${HEUR_SRC})
target_link_libraries (transgenetic5 graph sttree heuristic)

add_executable (transgenetic6 transgenetic.cpp version6.cpp ${HEUR_SRC})
target_link_libraries (transgenetic6 graph sttree heuristic)

add_executable (teste transgenetic.cpp main.cpp ${HEUR_SRC})
target_link_libraries (teste graph sttree heuristic)

#INSTALL(TARGETS transgenetic
#       RUNTIME
#        DESTINATION bin
#        PERMISSIONS OWNER_READ OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

FILE (GLOB SRCS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
FILE (GLOB HDRS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)
INSTALL (FILES ${SRCS} ${HDRS} CMakeLists.txt DESTINATION algorithm/transgenetic)
