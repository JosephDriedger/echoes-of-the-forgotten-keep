cmake_minimum_required(VERSION 4.1)

project(EchoesOfTheForgottenKeep VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

option(EFK_BUILD_TESTS "Build tests" OFF)

add_library(glad STATIC
        third_party/glad/src/gl.c
)

target_include_directories(glad
        PUBLIC
        ${CMAKE_SOURCE_DIR}/third_party/glad/include
)

set(EFK_SDL3_DIR ${CMAKE_SOURCE_DIR}/Dev/sdl3)
set(EFK_SDL3_IMAGE_DIR ${CMAKE_SOURCE_DIR}/Dev/sdl3_image)
set(EFK_ASSIMP_DIR ${CMAKE_SOURCE_DIR}/Dev/assimp-6.0.4)

add_subdirectory(engine)
add_subdirectory(game)

add_executable(EchoesOfTheForgottenKeep
        main.cpp
)

target_link_libraries(EchoesOfTheForgottenKeep
        PRIVATE
        efk_engine
        efk_game
)

add_custom_command(TARGET EchoesOfTheForgottenKeep POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/asset"
        "$<TARGET_FILE_DIR:EchoesOfTheForgottenKeep>/asset"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${EFK_SDL3_DIR}/bin/SDL3.dll"
        "$<TARGET_FILE_DIR:EchoesOfTheForgottenKeep>/SDL3.dll"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${EFK_SDL3_IMAGE_DIR}/bin/SDL3_image.dll"
        "$<TARGET_FILE_DIR:EchoesOfTheForgottenKeep>/SDL3_image.dll"

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${EFK_ASSIMP_DIR}/build/bin/libassimp-6.dll"
        "$<TARGET_FILE_DIR:EchoesOfTheForgottenKeep>/libassimp-6.dll"
)

if(EFK_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()