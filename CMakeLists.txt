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
results;
) 
INCLUDE(CPack)

#habilita test
enable_testing ()

project (Doutorado)

if( CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]" )
    message( "INFO: DEBUG BUILD" )
    # for debug type builds, turn on verbose makefiles
    SET(CMAKE_VERBOSE_MAKEFILE OFF)

    # Tell other CMake files that we're doing a debug build
    SET( DEBUG_BUILD 1 )

    # Tell C/C++ that we're doing a debug build
    ADD_DEFINITIONS( -DDEBUG )
    
    #set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")
endif()

if( CMAKE_BUILD_TYPE MATCHES "[Rr][Ee][Ll][Ee][Aa][Ss][Ee]" )
    message( "INFO: RELEASE BUILD" )
    
    set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_DEBUG} -O3")
endif()

add_definitions ("-std=c++0x -Wall")

set (EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/build")
set (LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

include_directories (${PROJECT_SOURCE_DIR})
include_directories (include lib)

add_subdirectory (algorithm)
add_subdirectory (network)
add_subdirectory (sttree)
add_subdirectory (statistics)
add_subdirectory (mpp_algorithms)
add_subdirectory (util)

INSTALL (FILES CMakeLists.txt DESTINATION .)
INSTALL (DIRECTORY include lib DESTINATION .)
INSTALL (DIRECTORY results DESTINATION .)

#my test
#adicionando teste
add_test (RUN ${PROJECT_BINARY_DIR}/build/chentest
	"../MPP_instances/n30/b30_1.brite" 
	"../MPP_instances/preprocessing/n30/b30_1.pre")