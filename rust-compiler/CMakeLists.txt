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

set(RUSTCC_LIB_SOURCES ${RUSTCC_SOURCES})
list(REMOVE_ITEM RUSTCC_LIB_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

add_library(rustcc_lib ${RUSTCC_LIB_SOURCES} ${RUSTCC_HEADERS})
target_include_directories(rustcc_lib PUBLIC include)

add_executable(rustcc ${RUSTCC_SOURCES} ${RUSTCC_HEADERS})

target_link_libraries(rustcc PRIVATE rustcc_lib argparse::argparse)

include(CTest)

if(BUILD_TESTING)
  add_subdirectory(tests)
endif()
