cmake_minimum_required(VERSION 3.20)

project(rust-compiler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  argparse
  GIT_REPOSITORY https://github.com/p-ranav/argparse.git
  GIT_TAG v2.9
)
FetchContent_MakeAvailable(argparse)

if(BUILD_TESTING)
  FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.12.1
  )
  FetchContent_MakeAvailable(googletest)
endif()

file(GLOB_RECURSE RUSTCC_SOURCES CONFIGURE_DEPENDS
  src/*.c
  src/*.cc
  src/*.cpp
  src/*.cxx
)
file(GLOB_RECURSE RUSTCC_HEADERS CONFIGURE_DEPENDS
  include/*.h
  include/*.hpp
)

add_executable(rustcc ${RUSTCC_SOURCES} ${RUSTCC_HEADERS})

target_include_directories(rustcc PRIVATE include)

target_link_libraries(rustcc PRIVATE argparse::argparse)

include(CTest)
