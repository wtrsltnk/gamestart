cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

include(cmake/CPM.cmake)

project(
    gamestart
    VERSION 1.0
    LANGUAGES CXX C
)

find_package(OpenGL REQUIRED)

CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    VERSION 1.7.0)

CPMAddPackage(
    NAME Clara
    GITHUB_REPOSITORY catchorg/Clara
    VERSION 1.1.5
    DOWNLOAD_ONLY True
)

if (Clara_ADDED)
    add_library(Clara INTERFACE)
    target_include_directories(Clara INTERFACE ${Clara_SOURCE_DIR}/single_include)
endif()

CPMAddPackage(
    NAME fmt
    GITHUB_REPOSITORY fmtlib/fmt
    GIT_TAG 7.1.0
)

CPMAddPackage(
    NAME glm
    GIT_TAG 0.9.9.7
    GITHUB_REPOSITORY g-truc/glm
)

CPMAddPackage(
    NAME SDL2
    VERSION 2.0.12
    URL https://libsdl.org/release/SDL2-2.0.12.zip
    OPTIONS
        "SDL_SHARED Off"
)

CPMAddPackage(
    NAME EnTT
    VERSION 3.5.2
    GITHUB_REPOSITORY skypjack/entt
    # EnTT's CMakeLists screws with configuration options
    DOWNLOAD_ONLY True
)

if (EnTT_ADDED)
    add_library(EnTT INTERFACE)
    target_include_directories(EnTT INTERFACE ${EnTT_SOURCE_DIR}/src)
endif()

add_executable(
    gamestart
    "src/application.cpp"
    "src/glad.c"
    "src/inputmanager.cpp"
    "src/main.cpp"
    "src/renderer.cpp"
    "src/scene.cpp"
)

target_include_directories(
    gamestart
    PRIVATE
        include
)

target_link_libraries(
    gamestart
    PRIVATE
        ${OPENGL_LIBRARIES}
        fmt
        glm
        spdlog
        SDL2-static
        EnTT
        Clara
)

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
