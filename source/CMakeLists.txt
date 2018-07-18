cmake_minimum_required (VERSION 3.11.0)
project(algo)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
execute_process(COMMAND /usr/bin/conan install ${CMAKE_SOURCE_DIR} --profile clang --build missing
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
execute_process(COMMAND /usr/bin/conan install ${CMAKE_SOURCE_DIR} --build missing
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
else()
  message(FATAL_ERROR "Compiler not configured ${CMAKE_CXX_COMPILER_ID}")
endif ()

set(CONAN_SYSTEM_INCLUDES "On")
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

enable_testing()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
-Wall \
-Wextra \
-Wconversion \
-Wfloat-equal \
-Wunreachable-code \
-Wswitch-enum \
-Wshadow \
-Wnon-virtual-dtor \
-std=c++17 \
-pedantic \
-g \
-Werror \
-Wno-unused-function")

#-Wsuggest-override \

add_subdirectory(src)
