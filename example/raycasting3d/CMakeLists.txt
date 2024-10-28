project(Raycasting3d)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(EMSCRIPTEN)
    message("Using Emscripten as the default compiler")
    if(NOT CMAKE_CXX_COMPILER)
        set(CMAKE_CXX_COMPILER em++)
    endif()
endif()

add_executable(raycasting3d raycasting3d.cpp)
target_link_libraries(raycasting3d SDL2main SDL2-static)

if(EMSCRIPTEN)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORTED_FUNCTIONS='[_main]'")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -s EXPORTED_RUNTIME_METHODS='[ccall, cwrap]'")
    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -03 -s USE_WEBGL2=1 -s FULL_ES3=1")
    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --preload-file ../assets")
    set_target_properties(raycasting3d PROPERTIES OUTPUT_NAME "../../pages/raycasting3d/raycasting3d" SUFFIX ".html")
endif()