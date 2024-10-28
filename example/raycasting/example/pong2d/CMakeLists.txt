project(Pong2D_Game)
set(PROJECT_NAME pong2d)


add_executable(${PROJECT_NAME} pong2d.cpp)

include_directories(${CMAKE_SOURCE_DIR}/deps/SDL/include)
include_directories(${CMAKE_SOURCE_DIR}/include)

if(EMSCRIPTEN)
    target_link_libraries(${PROJECT_NAME} SDL3-static)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORTED_FUNCTIONS='[_ymain, _setCanvasSize]'")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORTED_RUNTIME_METHODS='[ccall, cwrap]'" )
    #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -03" )

    set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "../../web/${PROJECT_NAME}/${PROJECT_NAME}" SUFFIX ".js")
else()
    target_link_libraries(${PROJECT_NAME} SDL3-shared)
endif()