cmake_minimum_required(VERSION 3.25)

set(DL2Hub_SimpleGames)

include_directories(${CMAKE_SOURCE_DIR}/include)

add_executable(pong2d pong2d.cpp)
add_executable(main main.cpp)

target_link_libraries(pong2d SDL3-shared)