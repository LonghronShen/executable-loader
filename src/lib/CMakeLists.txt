cmake_minimum_required(VERSION 3.16)
project(${project_name}-lib C CXX)

file(GLOB_RECURSE src
    ${CMAKE_CURRENT_LIST_DIR}/*.h
    ${CMAKE_CURRENT_LIST_DIR}/*.hpp

    ${CMAKE_CURRENT_LIST_DIR}/*.c
	${CMAKE_CURRENT_LIST_DIR}/*.cc
	${CMAKE_CURRENT_LIST_DIR}/*.cpp
)

file(GLOB_RECURSE public_headers
    ${CMAKE_CURRENT_LIST_DIR}/include/*.h
    ${CMAKE_CURRENT_LIST_DIR}/include/*.hpp
)

include_directories(
    ${CMAKE_CURRENT_LIST_DIR}/include
)

add_library(${project_name}-lib
    STATIC ${src}
)

target_link_libraries(${project_name}-lib
    PUBLIC ${CMAKE_THREAD_LIBS_INIT}
    PUBLIC ${DL_LIBRARY}
    PUBLIC ${Boost_LIBRARIES}
    PUBLIC utf8cpp
    PUBLIC crossguid
)

target_include_directories(${project_name}-lib
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include/public"
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include/private"
    PUBLIC "${CMAKE_CURRENT_BINARY_DIR}/include"
)
