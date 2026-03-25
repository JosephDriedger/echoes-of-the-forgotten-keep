cmake_minimum_required(VERSION 4.1)

file(GLOB_RECURSE EFK_ENGINE_HEADERS CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

file(GLOB_RECURSE EFK_ENGINE_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(efk_engine STATIC
        ${EFK_ENGINE_HEADERS}
        ${EFK_ENGINE_SOURCES}
)

target_include_directories(efk_engine
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(efk_engine
        PUBLIC
        efk::dependencies
        glad
)

target_compile_features(efk_engine PUBLIC cxx_std_20)

target_compile_definitions(efk_engine
        PRIVATE
        EFK_ENGINE_BUILD
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
        ${EFK_ENGINE_HEADERS}
        ${EFK_ENGINE_SOURCES}
)

set_target_properties(efk_engine PROPERTIES
        OUTPUT_NAME engine
)