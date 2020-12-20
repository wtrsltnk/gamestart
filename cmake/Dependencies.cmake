CPMAddPackage(
    NAME spdlog
    GITHUB_REPOSITORY gabime/spdlog
    VERSION 1.7.0
)

CPMAddPackage(
    NAME Lyra
    GITHUB_REPOSITORY bfgroup/Lyra
    GIT_TAG 1.5.1
    DOWNLOAD_ONLY True
)

if (Lyra_ADDED)
    add_library(Lyra INTERFACE)
    target_include_directories(Lyra INTERFACE ${Lyra_SOURCE_DIR}/include)
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
    URL https://www.libsdl.org/release/SDL2-2.0.12.zip
    OPTIONS
        "SDL_SHARED Off"
)

CPMAddPackage(
    NAME EnTT
    GIT_TAG v3.5.2
    GITHUB_REPOSITORY skypjack/entt
    # EnTT's CMakeLists screws with configuration options
    DOWNLOAD_ONLY True
)

if (EnTT_ADDED)
    add_library(EnTT INTERFACE)
    target_include_directories(EnTT INTERFACE ${EnTT_SOURCE_DIR}/src)
endif()

CPMAddPackage(
    NAME imgui
    GIT_TAG v1.79
    GITHUB_REPOSITORY ocornut/imgui
    DOWNLOAD_ONLY True
)

if (imgui_ADDED)
    add_library(imgui)
    target_include_directories(
        imgui
        PUBLIC
            "${imgui_SOURCE_DIR}/"
            "${imgui_SOURCE_DIR}/examples/"
    )

    target_sources(
        imgui
        PUBLIC
            "${imgui_SOURCE_DIR}/imgui.cpp"
            "${imgui_SOURCE_DIR}/imgui_demo.cpp"
            "${imgui_SOURCE_DIR}/imgui_draw.cpp"
            "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    )
endif()
