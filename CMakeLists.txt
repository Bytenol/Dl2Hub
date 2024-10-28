cmake_minimum_required(VERSION 3.21.0)
project(SDL2Hub)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(deps/SDL-release-2.30.7)
add_subdirectory(example/tetris)
add_subdirectory(example/raycasting3d)

include_directories(deps/SDL-release-2.30.7/include)
include_directories(include)

if(EMSCRIPTEN)
else()
    add_executable(main_test example/main_test.cpp)
endif()

include(CTest)
enable_testing()