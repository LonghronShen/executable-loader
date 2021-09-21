find_package(Threads REQUIRED)

if(UNIX)
    find_package(DL REQUIRED)
endif()

include(FetchContent)

# googletest
option(EXECUTABLE_LOADER_BUILD_GTEST "Build Gtest for EXECUTABLE_LOADER_BUILD_GTEST." ON)
message(STATUS "EXECUTABLE_LOADER_BUILD_GTEST:${EXECUTABLE_LOADER_BUILD_GTEST}")
if(EXECUTABLE_LOADER_BUILD_GTEST)
    FetchContent_Declare(googletest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG release-1.10.0)

    FetchContent_GetProperties(googletest)
    if(NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
        include(GoogleTest)
    endif()
endif()

# boost-cmake
if(WIN32)
    set(Boost_USE_STATIC_LIBS ON CACHE STRING "Boost_USE_STATIC_LIBS" FORCE)
    set(Boost_USE_STATIC_RUNTIME ON CACHE STRING "Boost_USE_STATIC_RUNTIME" FORCE)
endif()

find_package(Boost 1.66 REQUIRED COMPONENTS thread log log_setup system program_options filesystem coroutine locale regex unit_test_framework)
if(Boost_FOUND)
    message(STATUS "** Boost Include: ${Boost_INCLUDE_DIR}")
    message(STATUS "** Boost Libraries Directory: ${Boost_LIBRARY_DIRS}")
    message(STATUS "** Boost Libraries: ${Boost_LIBRARIES}")
    include_directories(${Boost_INCLUDE_DIRS})
else()
    if(WIN32)
        message(FATAL_ERROR "Plase check your vcpkg settings or global environment variables for the boost library.")
    else()
        FetchContent_Declare(boost_cmake
            GIT_REPOSITORY https://github.com/Orphis/boost-cmake.git
            GIT_TAG d3951bc7f0b9d09005f92aedcf6acfc595f050ea)

        FetchContent_GetProperties(boost_cmake)
        if(NOT boost_cmake_POPULATED)
            FetchContent_Populate(boost_cmake)
            add_subdirectory(${boost_cmake_SOURCE_DIR} ${boost_cmake_BINARY_DIR} EXCLUDE_FROM_ALL)
        endif()
    endif()
endif()

# boost.predef
FetchContent_Declare(predef
  GIT_REPOSITORY https://github.com/boostorg/predef.git
  GIT_TAG boost-1.74.0)

FetchContent_GetProperties(predef)
if(NOT predef_POPULATED)
  FetchContent_Populate(predef)
  add_subdirectory(${predef_SOURCE_DIR} ${predef_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# utf8cpp
FetchContent_Declare(utf8cpp
  GIT_REPOSITORY https://github.com/nemtrif/utfcpp.git
  GIT_TAG v3.2)

FetchContent_GetProperties(utf8cpp)
if(NOT utf8cpp_POPULATED)
  FetchContent_Populate(utf8cpp)
  add_subdirectory(${utf8cpp_SOURCE_DIR} ${utf8cpp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

# crossguid
set(XG_TESTS OFF CACHE BOOL "XG_TESTS" FORCE)
set(CROSS_GUID_WARNINGS "" CACHE BOOL "CROSS_GUID_WARNINGS" FORCE)

FetchContent_Declare(crossguid
  GIT_REPOSITORY https://github.com/LonghronShen/crossguid.git
  GIT_TAG master)

FetchContent_GetProperties(crossguid)
if(NOT crossguid_POPULATED)
  FetchContent_Populate(crossguid)
  add_subdirectory(${crossguid_SOURCE_DIR} ${crossguid_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
