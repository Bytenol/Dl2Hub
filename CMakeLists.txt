cmake_minimum_required(VERSION 3.21.0)
project(cpp VERSION 0.1.0 LANGUAGES C CXX)


set(USE_OFFLINE ON)

if(${USE_OFFLINE})
    set(SDL2_LIB_FILE ${CMAKE_SOURCE_DIR}/deps/SDL-release-2.28.1.zip)
else()
    set(SDL2_LIB_FILE "To be set later")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS 0FF)

add_compile_options(-std=c++20)

include(FetchContent)

FetchContent_Declare(
    SDL2
    URL
    ${SDL2_LIB_FILE}
)

FetchContent_MakeAvailable(SDL2)


add_subdirectory(example/SimpleGames)

include(CTest)
enable_testing()

