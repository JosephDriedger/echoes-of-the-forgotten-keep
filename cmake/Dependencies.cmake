include(FetchContent)

set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/.deps CACHE PATH "FetchContent base directory")

#
# SDL3
#
find_package(SDL3 CONFIG QUIET)

if(NOT SDL3_FOUND)
    message(STATUS "SDL3 not found. Fetching SDL3...")

    FetchContent_Declare(
            SDL3
            GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
            GIT_TAG release-3.2.28
            GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(SDL3)
endif()

#
# GLM
#
find_package(glm CONFIG QUIET)

if(NOT glm_FOUND)
    message(STATUS "GLM not found. Fetching GLM...")

    FetchContent_Declare(
            glm
            GIT_REPOSITORY https://github.com/g-truc/glm.git
            GIT_TAG 1.0.1
            GIT_SHALLOW TRUE
    )

    FetchContent_MakeAvailable(glm)
endif()

#
# OpenGL
#
find_package(OpenGL REQUIRED)

#
# Shared dependency interface
#
add_library(efk_dependencies INTERFACE)
add_library(efk::dependencies ALIAS efk_dependencies)

target_link_libraries(efk_dependencies
        INTERFACE
        SDL3::SDL3
        glm::glm
        OpenGL::GL
)