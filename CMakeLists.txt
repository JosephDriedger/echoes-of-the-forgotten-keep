cmake_minimum_required(VERSION 4.1)

project(EchoesOfTheForgottenKeep VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

option(EFK_BUILD_TESTS "Build tests" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(Dependencies)

add_library(glad STATIC
        third_party/glad/src/gl.c
)

target_include_directories(glad
        PUBLIC
        ${CMAKE_SOURCE_DIR}/third_party/glad/include
)

add_subdirectory(engine)
add_subdirectory(game)

add_executable(${PROJECT_NAME}
        main.cpp
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        efk_engine
        efk_game
        glad
)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/game/assets
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/assets
)

if(EFK_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()