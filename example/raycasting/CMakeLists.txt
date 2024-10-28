cmake_minimum_required(VERSION 3.19.0)
project(rayCasting3d VERSION 0.1.0 LANGUAGES CXX)

add_subdirectory(deps/SDL-release-2.30.5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(rayCasting3d main.cpp)

include_directories(deps/SDL-release-2.30.5/include)

target_link_libraries(rayCasting3d SDL2main SDL2-static)

