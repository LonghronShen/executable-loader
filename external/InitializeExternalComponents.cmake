find_package(Threads REQUIRED)

if(UNIX)
    find_package(DL REQUIRED)
endif()

include(FetchContent)

# googletest
FetchContent_Declare(googletest
  GIT_REPOSITORY https://github.com/google/googletest
  GIT_TAG release-1.10.0)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
  include(GoogleTest)
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

FetchContent_Declare(crossguid
  GIT_REPOSITORY https://github.com/graeme-hill/crossguid.git
  GIT_TAG ca1bf4b810e2d188d04cb6286f957008ee1b7681)

FetchContent_GetProperties(crossguid)
if(NOT crossguid_POPULATED)
  FetchContent_Populate(crossguid)
  add_subdirectory(${crossguid_SOURCE_DIR} ${crossguid_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()