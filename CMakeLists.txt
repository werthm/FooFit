cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(FooFit)

# more warnings
add_definitions(-D_FORTIFY_SOURCE=2)
add_definitions(-Wall)

# set default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# Mac OS X special settings
if(APPLE)
    set(CMAKE_MACOSX_RPATH 1)
    set(CMAKE_SHARED_LIBRARY_SUFFIX ".so")
endif(APPLE)

# find ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})
find_package(ROOT REQUIRED COMPONENTS MathCore RIO Hist Tree RooFit RooFitCore RooStats)
include(${ROOT_USE_FILE})

# source file globbing
file(GLOB SRCS src/FF*.cxx)

# header directory
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# create the dictionary
file(GLOB HDR ${CMAKE_CURRENT_SOURCE_DIR}/include/FF*.h)
foreach(head_file ${HDR})
    # extract pure header file name
    get_filename_component(pure_hdr ${head_file} NAME)
    set(HEADERS ${HEADERS} ${pure_hdr})
endforeach()

add_custom_command(OUTPUT G__FooFit.cxx
    COMMAND rootcling -f G__FooFit.cxx
                      -rml libFooFit.so
                      -rmf libFooFit.rootmap
                      -s libFooFit.so
                      -I${CMAKE_CURRENT_SOURCE_DIR}/include
                      ${HEADERS}
                      ${CMAKE_CURRENT_SOURCE_DIR}/include/LinkDef.h
)

# create the shared library
add_library(FooFit SHARED ${SRCS} G__FooFit.cxx)
target_link_libraries(FooFit ${ROOT_LIBRARIES})

# installation
install(TARGETS FooFit LIBRARY DESTINATION ${PROJECT_BINARY_DIR}/lib)
install(FILES ${PROJECT_BINARY_DIR}/libFooFit.rootmap DESTINATION ${PROJECT_BINARY_DIR}/lib)
install(FILES ${PROJECT_BINARY_DIR}/libFooFit_rdict.pcm DESTINATION ${PROJECT_BINARY_DIR}/lib)
install(FILES ${HDR} DESTINATION ${PROJECT_BINARY_DIR}/include)

