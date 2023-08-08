cmake_minimum_required(VERSION 3.16)
project(mimir)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

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

Include(FetchContent)
message(STATUS "Fetching catch2...")
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0 # or a later release
)
FetchContent_MakeAvailable(Catch2)

message(STATUS "Fetching fmt...")
FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG       9.1.0
)
FetchContent_MakeAvailable(fmt)

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
