cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(FooFit)

# use copied CMake modules from ROOT 6
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "$ENV{ROOTSYS}/etc/cmake/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

# adjust some paths
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED MODULE COMPONENTS MathCore RIO Hist Tree Net RooFit RooStats)

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

# copy headers to build directory
add_custom_command(TARGET FooFit POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/include ${PROJECT_BINARY_DIR}/include
)

