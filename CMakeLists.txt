cmake_minimum_required(VERSION 3.21.0)
project(cpp VERSION 0.1.0 LANGUAGES C CXX)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(deps/SDL)

add_subdirectory(example/SimpleGames)

include_directories(deps/SDL/include)

include(CTest)
enable_testing()