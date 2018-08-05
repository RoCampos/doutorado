cmake_minimum_required (VERSION 2.8.12.2)

set(VERSION_MAJOR   3   CACHE STRING "Project major version number.")
set(VERSION_MINOR   0   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

SET (CPACK_PACKAGE_CONTACT romerito.campos@gmail.com)
SET (CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
SET (CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
SET (CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
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
#enable_testing ()

project (Doutorado)

#SET (CMAKE_CXX_COMPILER "/usr/bin/c++")
set (CMAKE_CXX_FLAGS "-std=c++11")

if( CMAKE_BUILD_TYPE MATCHES "[Dd][Ee][Bb][Uu][Gg]" )
    message( "INFO: DEBUG BUILD" )
    # for debug type builds, turn on verbose makefiles
    SET(CMAKE_VERBOSE_MAKEFILE OFF)

    # Tell other CMake files that we're doing a debug build
    SET( DEBUG_BUILD 1 )

    # Tell C/C++ that we're doing a debug build
    ADD_DEFINITIONS( -DDEBUG ) 
    add_definitions ("-std=c++11 -Wall")
    message("${CMAKE_CXX_FLAGS_DEBUG}")
    
endif()

if( CMAKE_BUILD_TYPE MATCHES "[Rr][Ee][Ll][Ee][Aa][Ss][Ee]" )
    message( "INFO: RELEASE BUILD" )
    set (CMAKE_VERBOSE_MAKEFILE ON)    
    add_definitions ("-std=c++11 -stdlib=libc++ -Wall")    
    message("${CMAKE_CXX_FLAGS_RELEASE}")
endif()

if (CMAKE_BUILD_TYPE MATCHES "[Pp][Rr][Oo][Ff][Ii][Ll][Ee]")
    message ("INFO: PROFILE BUILD")

    set (CMAKE_VERBOSE_MAKEFILE ON)    
    set (CMAKE_CXX_FLAGS_PROFILE "-O0 -pg")    
    add_definitions ("-std=c++11")
    
    message("${CMAKE_CXX_FLAGS_PROFILE}")
    
endif()

message (" ${CMAKE_CXX_COMPILER}")

# ------------------------ config versio file ----------------- #
configure_file (
  "${PROJECT_BINARY_DIR}/include/config.h.in"
  "${PROJECT_BINARY_DIR}/include/config.h"
  )
# --------------------------- endof config file --------------- #


# ------------------------ FIND_BOOST ------------------------- #
set(Boost_USE_STATIC_LIBS        ON)
set(Boost_USE_MULTITHREADED      OFF)
set(Boost_USE_STATIC_RUNTIME    OFF)
find_package (Boost COMPONENTS regex REQUIRED)
if (Boost_FOUND)    
    MESSAGE ("BOOST WAS FOUND!")
    MESSAGE ("${Boost_INCLUDE_DIR}")
    include_directories(${Boost_INCLUDE_DIR})
    add_definitions ("-DHASBOOT")
endif()

message ("${BOOST_ROOT}")

# ---------------------- END OF BOOST SEARC ------------------- #

#------------------------ FIND_GUROBI_LIB --------------------- #
if (GUROBI_INCLUDE_DIR)

    set (GUROBI_FOUND TRUE)
    set (GUROBI_INCLUDE_DIRS "${GUROBI_INCLUDE_DIR}")
    set (GUROBI_LIBRARIES "${GUROBI_LIBRARY};${GUROBI_CXX_LIBRARY}")
    set (GUROBI_CXX_LIBRARY "${GUROBI_CXX_LIBRARY}")

else (GUROBI_INCLUDE_DIRS)
    find_path (GUROBI_INCLUDE_DIR
        NAMES "gurobi_c++.h" "gurobi_c.h"
        PATHS 
            "$ENV{GUROBI_HOME}/include"
            "/Library/gurobi702/mac64/include"
        )

    find_library (GUROBI_LIBRARY
        NAMES 
            "gurobi" 
            "gurobi70"
        PATHS   
            "$ENV{GUROBI_HOME}/lib"
            "/Library/gurobi702/mac64/lib"
       )

    find_library (GUROBI_CXX_LIBRARY
        NAMES 
            "gurobi_c"
            "gurobi_c++" 
            "gurobi_g++4.1" 
            "gurobi_g++4.2" 
        PATHS   
            "$ENV{GUROBI_HOME}/lib"
            "/Library/gurobi702/mac64/lib"
        )

    set(GUROBI_INCLUDE_DIRS "${GUROBI_INCLUDE_DIR}")
    set(GUROBI_LIBRARIES "${GUROBI_LIBRARY};${GUROBI_CXX_LIBRARY}")
    set(GUROBI_CXX_LIBRARY "${GUROBI_CXX_LIBRARY}")

    # testing if the variables were found
    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(GUROBI  DEFAULT_MSG
        GUROBI_LIBRARY GUROBI_CXX_LIBRARY GUROBI_INCLUDE_DIR)

    mark_as_advanced(GUROBI_INCLUDE_DIR GUROBI_LIBRARY GUROBI_CXX_LIBRARY)

endif (GUROBI_INCLUDE_DIR)
#------------------------ ENDOFGUROBI ------------------------- #

message ("${GUROBI_INCLUDE_DIR}")

#--------------------------- find omp
find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
#------------------------- end find omp


# ----------------------- FIND DOXYGEM ------------------------ # 
find_package (Doxygen)
option(BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

if (BUILD_DOCUMENTATION)
	
	if(NOT DOXYGEN_FOUND)
		message (FATAL_ERROR "Doxygen is need to build docs")
	endif()

	set (doxyfile ${PROJECT_SOURCE_DIR}/scripts/Doxyfile)

	add_custom_target (doc
		COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
		WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/docs
		COMMENT "Generating API documentation with Doxigen"
		VERBATIM)

	#install (DIRECTORY docs/html DESTINATION ./share/doc)

endif()

# ----------------------- ENDOFDOXYGEN ------------------------ #

set (EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/build")
set (LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib")

#GLOBAL SETTING OF HOW TO READ THE NETWORK
set (REAL ON)
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
#add_subdirectory (mpp_algorithms)
add_subdirectory (util)
add_subdirectory (stalgorithm)
add_subdirectory (mmmstp) #multiple tree under budget constraints
add_subdirectory (mmstp-hop) #multiple tree under hop constraints
add_subdirectory (multi)

#directory for test of libraries and some mpp_algorithms
#add_subdirectory (testes)

INSTALL (FILES CMakeLists.txt DESTINATION .)
INSTALL (DIRECTORY include lib DESTINATION .)
INSTALL (DIRECTORY scripts DESTINATION .)

#my test
#adicionando teste
add_test (RUN ${PROJECT_BINARY_DIR}/build/chentest
	"../MPP_instances/n30/b30_1.brite" 
	"../MPP_instances/preprocessing/n30/b30_1.pre")
