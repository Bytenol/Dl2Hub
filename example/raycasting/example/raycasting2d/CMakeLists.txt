project(Raycasting2d_Game)
set(PROJECT_NAME raycasting2d)


add_executable(${PROJECT_NAME} raycasting2d.cpp)

include_directories(${CMAKE_SOURCE_DIR}/deps/SDL/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

if(EMSCRIPTEN)
    target_link_libraries(${PROJECT_NAME} SDL3-static)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORTED_FUNCTIONS='[_main]'")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORTED_RUNTIME_METHODS='[ccall, cwrap]'" )
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s NO_DISABLE_EXCEPTION_CATCHING -s ALLOW_MEMORY_GROWTH=1" )

    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "../../web/${PROJECT_NAME}/${PROJECT_NAME}" SUFFIX ".html")
else()
    target_link_libraries(${PROJECT_NAME} SDL3-shared)
endif()