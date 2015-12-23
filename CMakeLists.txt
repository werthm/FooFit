cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(FooFit)

# more warnings
add_definitions(-D_FORTIFY_SOURCE=2)

# set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# define variables
set(DOCS ON CACHE BOOL "create FooFit HTML documentation")

# Mac OS X special settings
if(APPLE)
    set(CMAKE_MACOSX_RPATH 1)
    set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

# use copied CMake modules from ROOT 6
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{ROOTSYS}/etc/cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

# adjust some paths
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED MODULE COMPONENTS MathCore RIO Hist Tree Net RooFit RooFitCore RooStats)

# check for RooFit support
if(NOT ROOT_roofit_FOUND)
    message(FATAL_ERROR "No RooFit support in ROOT installation!")
endif()

# define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

# source file globbing
file(GLOB SRCS src/*.cxx)

# header directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# create the dictionary
ROOT_GENERATE_DICTIONARY(G__FooFit include/FF*.h LINKDEF LinkDef.h)

# create the shared library
add_library(FooFit SHARED ${SRCS} G__FooFit.cxx)
target_link_libraries(FooFit ${ROOT_LIBRARIES})

# generate rootmap and copy headers to build directory
add_custom_command(TARGET FooFit POST_BUILD 
    COMMAND rlibmap -o ${PROJECT_BINARY_DIR}/lib/libFooFit.rootmap
                    -l ${PROJECT_BINARY_DIR}/lib/libFooFit.so
                    -d ${ROOT_LIBRARIES}
                    -c ${CMAKE_SOURCE_DIR}/include/LinkDef.h
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}/include
)

# generate HTML documentation
if (DOCS)
    add_custom_command(TARGET FooFit POST_BUILD
        COMMAND root -b ${CMAKE_SOURCE_DIR}/src/htmldoc.C'(\"${CMAKE_SOURCE_DIR}\") '
    )
endif()

