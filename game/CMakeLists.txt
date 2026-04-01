cmake_minimum_required(VERSION 4.1)

file(GLOB_RECURSE EFK_GAME_HEADERS CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
)

file(GLOB_RECURSE EFK_GAME_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
)

add_library(efk_game STATIC
        ${EFK_GAME_HEADERS}
        ${EFK_GAME_SOURCES}
        include/game/ui/UIRenderer.h
        src/ui/UIRenderer.cpp
)

target_include_directories(efk_game
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/engine/include
)

target_link_libraries(efk_game
        PUBLIC
        efk_engine
)

target_compile_features(efk_game PUBLIC cxx_std_20)

target_compile_definitions(efk_game
        PRIVATE
        EFK_GAME_BUILD
)

source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES
        ${EFK_GAME_HEADERS}
        ${EFK_GAME_SOURCES}
)

set_target_properties(efk_game PROPERTIES
        OUTPUT_NAME game
)