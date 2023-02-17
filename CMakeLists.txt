cmake_minimum_required(VERSION 3.16)
project(lpp_interpreter)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(${CMAKE_SOURCE_DIR}/build/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

if(MSVC)
    set(CPP_LINKING_OPTS /fsanitize=address)
else()
    set(CPP_LINKING_OPTS -fsanitize=undefined,address -fno-omit-frame-pointer)
endif()

if(MSVC)
    set(CPP_FLAGS /W4 /permissive-)
else()
    set(CPP_FLAGS
        -Wall
        -Wextra
        -Wconversion
        -Wpedantic
        -Wshadow
        -Wnon-virtual-dtor
        -Wsign-conversion
        -Wcast-align
        -Wold-style-cast
        -Wimplicit-fallthrough)
endif()

add_subdirectory(src)

if("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    enable_testing()
    add_subdirectory(tests)
endif()

if(MSVC)
  add_compile_options("$<$<CONFIG:Debug>:/DEBUG;/Od>")
  add_compile_options("$<$<CONFIG:Release>:/O2>")
else()
  add_compile_options("$<$<CONFIG:Debug>:-O0;-g3;-ggdb>")
  add_compile_options("$<$<CONFIG:Release>:-O2>")
endif()
