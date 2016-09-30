cmake_minimum_required(VERSION 2.8)
project(GraphMatching)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -g ")
#-z execstack
#--run hs --command-template="insight %s

set(SOURCE_DIR code/source/)
set(HEADER_DIR code/include/)

include_directories(${HEADER_DIR})

#set(HEADER_FILES
#        definitions.h
#        edge.h
#        graph.h
#        rg.h)

set(SOURCE_FILES
        code/main/main.cpp
        ${SOURCE_DIR}edge.cpp
        ${SOURCE_DIR}graph.cpp
        ${SOURCE_DIR}rg.cpp code/include/dsp.h code/main/test.cpp code/main/test.h)

add_executable(GraphMatching ${SOURCE_FILES})

#set_target_properties(GraphMatching PROPERTIES LINKER_LANGUAGE CXX)