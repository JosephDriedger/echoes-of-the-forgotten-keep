cmake_minimum_required(VERSION 4.1)

project(echoes_of_the_forgotten_keep LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# --------------------------------------------------
# Third-party dependency locations
# --------------------------------------------------

set(EFK_SDL3_DIR "${CMAKE_SOURCE_DIR}/third_party/sdl3")
set(EFK_SDL3_IMAGE_DIR "${CMAKE_SOURCE_DIR}/third_party/sdl3_image")
set(EFK_ASSIMP_DIR "${CMAKE_SOURCE_DIR}/third_party/assimp-6.0.4")

if(NOT EXISTS "${EFK_SDL3_DIR}")
    message(FATAL_ERROR "SDL3 directory not found: ${EFK_SDL3_DIR}")
endif()

if(NOT EXISTS "${EFK_SDL3_IMAGE_DIR}")
    message(FATAL_ERROR "SDL3_image directory not found: ${EFK_SDL3_IMAGE_DIR}")
endif()

if(NOT EXISTS "${EFK_ASSIMP_DIR}")
    message(FATAL_ERROR "Assimp directory not found: ${EFK_ASSIMP_DIR}")
endif()

# --------------------------------------------------
# glad
# --------------------------------------------------

add_library(glad STATIC
        third_party/glad/src/gl.c
)

target_include_directories(glad
        PUBLIC
        ${CMAKE_SOURCE_DIR}/third_party/glad/include
)

# --------------------------------------------------
# Project modules
# --------------------------------------------------

add_subdirectory(engine)
add_subdirectory(game)
add_subdirectory(tests)

# --------------------------------------------------
# Main executable
# --------------------------------------------------

add_executable(echoes_of_the_forgotten_keep
        main.cpp
)

target_link_libraries(echoes_of_the_forgotten_keep
        PRIVATE
        efk_game
)

if(WIN32)
    target_link_libraries(echoes_of_the_forgotten_keep
            PRIVATE
            opengl32
    )
endif()

# --------------------------------------------------
# Post-build: copy DLLs and assets to output directory
# --------------------------------------------------

set(EFK_RUNTIME_DLLS
        "${EFK_SDL3_DIR}/bin/SDL3.dll"
        "${EFK_SDL3_IMAGE_DIR}/bin/SDL3_image.dll"
        "${EFK_ASSIMP_DIR}/build/bin/libassimp-6.dll"
)

add_custom_command(TARGET echoes_of_the_forgotten_keep POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${EFK_RUNTIME_DLLS}
        $<TARGET_FILE_DIR:echoes_of_the_forgotten_keep>
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/asset"
        "$<TARGET_FILE_DIR:echoes_of_the_forgotten_keep>/asset"
        COMMENT "Copying runtime DLLs and assets..."
)
