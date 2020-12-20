cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(cmake/CPM.cmake)

project(
    gamestart
    VERSION 1.0
    LANGUAGES CXX C
)

find_package(OpenGL REQUIRED)

include(cmake/Dependencies.cmake)

add_library(
    tiny_obj_loader
    "src/thirdparty/tiny_obj_loader.cpp"
    "src/thirdparty/tiny_obj_loader.h"
)

target_include_directories(
    tiny_obj_loader
    PUBLIC
        src/thirdparty
)

add_executable(
    gamestart
    "include/entities/graphicscomponent.h"
    "include/entities/namecomponent.h"
    "include/entities/transformcomponent.h"
    "src/core/application.cpp"
    "include/core/application.h"
    "include/core/assetsmanager.h"
    "src/core/assetsmanager.cpp"
    "src/core/drawobject.cpp"
    "include/core/drawobject.h"
    "src/core/glad.c"
    "src/core/imguilayer.cpp"
    "include/core/imguilayer.h"
    "src/core/imgui_impl_opengl3.cpp"
    "src/core/imgui_impl_opengl3.h"
    "src/core/inputmanager.cpp"
    "include/core/inputmanager.h"
    "src/core/layer.cpp"
    "include/core/layer.h"
    "src/core/gamelayer.cpp"
    "include/core/gamelayer.h"
    "src/main.cpp"
    "src/renderer.cpp"
    "include/renderer.h"
    "src/scene.cpp"
    "include/scene.h"
)

target_include_directories(
    gamestart
    PRIVATE
        include
)

target_link_libraries(
    gamestart
    PRIVATE
        tiny_obj_loader
        ${OPENGL_LIBRARIES}
        fmt
        glm
        spdlog
        EnTT
        Lyra
        imgui
)

if (WIN32)

    target_link_libraries(
        gamestart
        PRIVATE
            SDL2-static)

else()

    target_link_libraries(
        gamestart
        PRIVATE
            SDL2)

endif()

target_compile_features(
    gamestart
    PRIVATE
        cxx_std_17
        cxx_auto_type
        cxx_nullptr
        cxx_range_for
        cxx_thread_local
)

if (WIN32)
    target_sources(
        gamestart
        PRIVATE
            "src/application.win32.cpp"
    )
endif()
