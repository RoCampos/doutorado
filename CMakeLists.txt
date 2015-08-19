cmake_minimum_required (VERSION 2.8.12.2)

SET (CPACK_PACKAGE_CONTACT romerito.campos@gmail.com)
SET (CPACK_PACKAGE_VERSION_MAJOR 0)
SET (CPACK_PACKAGE_VERSION_MINOR 4)
SET (CPACK_PACKAGE_VERSION_PATCH 5)
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
    add_definitions ("-std=c++0x -Wall")
    message("${CMAKE_CXX_FLAGS_DEBUG}")
    
endif()

if( CMAKE_BUILD_TYPE MATCHES "[Rr][Ee][Ll][Ee][Aa][Ss][Ee]" )
    message( "INFO: RELEASE BUILD" )    
    add_definitions ("-std=c++0x -Wall")    
    message("${CMAKE_CXX_FLAGS_RELEASE}")
endif()

if (CMAKE_BUILD_TYPE MATCHES "[Pp][Rr][Oo][Ff][Ii][Ll][Ee]")
    message ("INFO: PROFILE BUILD")
    
    set (CMAKE_CXX_FLAGS_PROFILE "-O0 -pg")    
    add_definitions ("-std=c++0x")
    
    message("${CMAKE_CXX_FLAGS_PROFILE}")
    
endif()

#installation of boost libs
find_package (Boost)
if (Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIR})
  add_definitions ("-DHASBOOT")
endif()

set (EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/build")
set (LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")


#GLOBAL SETTING OF HOW TO READ THE NETWORK
set (REAL OFF)
IF (REAL)
	message ("\nINFO: REAL=${REAL} means Reader uses real values of instance")
ELSE()
	message ("\nINFO: REAL=${REAL} means Reader uses real values of instance")
ENDIF()

include_directories (${PROJECT_SOURCE_DIR})
include_directories (include lib)

#add_subdirectory (algorithm)
add_subdirectory (network)
add_subdirectory (sttree)
add_subdirectory (statistics)
add_subdirectory (mpp_algorithms)
add_subdirectory (util)
add_subdirectory (stalgorithm)
add_subdirectory (mmmstp)
add_subdirectory (multi)

#directory for test of libraries and some mpp_algorithms
add_subdirectory (testes)

INSTALL (FILES CMakeLists.txt DESTINATION .)
INSTALL (DIRECTORY include lib DESTINATION .)
INSTALL (DIRECTORY scripts DESTINATION .)

#my test
#adicionando teste
add_test (RUN ${PROJECT_BINARY_DIR}/build/chentest
	"../MPP_instances/n30/b30_1.brite" 
	"../MPP_instances/preprocessing/n30/b30_1.pre")
