cmake_minimum_required (VERSION 2.8.7)

SET (CPACK_PACKAGE_CONTACT romerito.campos@gmail.com)
SET (CPACK_PACKAGE_VERSION_MAJOR 0)
SET (CPACK_PACKAGE_VERSION_MINOR 1)
SET (CPACK_PACKAGE_VERSION_PATCH 0)
SET (CPACK_PACKAGE_VERSION ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH})
SET (CPACK_PACKAGE_NAME doutorado)
SET (CPACK_PACKAGE_VENDOR "romerito campos")
SET (CPACK_SOURCE_GENERATOR TGZ)
SET (CPACK_INSTALL_DIRECTORY doutorado)
SET (CPACK_SOURCE_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION})
SET (CPACK_PACKAGE_FILE_NAME ${CPACK_SOURCE_PACKAGE_FILE_NAME})
SET (CPACK_CMAKE_GENERATOR "Unix Makefiles")

#rules to packaging only needed files do delivery the project
SET(CPACK_SOURCE_IGNORE_FILES
_CPack_Packages;
doutorado*;
CMakeFiles;
CPack*;
cmake_install*;
CMakeCache*;
install*;
) 
INCLUDE(CPack)

project (Doutorado)

set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -O3")

add_definitions ("-std=c++0x -Wall")

set (EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/build")
set (LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

include_directories (${PROJECT_SOURCE_DIR})
include_directories (include lib)

add_subdirectory (algorithm)
add_subdirectory (network)
add_subdirectory (sttree)
add_subdirectory (statistics)

INSTALL (FILES CMakeLists.txt DESTINATION .)
INSTALL (DIRECTORY include lib DESTINATION .)
INSTALL (DIRECTORY results DESTINATION .)

