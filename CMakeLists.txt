cmake_minimum_required(VERSION 4.1)
project(echoes_of_the_forgotten_keep)

set(CMAKE_CXX_STANDARD 20)

#set SDL3 include and lib paths

if (WIN32)
    set(SDL3_PATH "/Dev/sdl3")
    set(SDL3_IMAGE_PATH "/Dev/sdl3_image")
    set(ASSIMP_PATH "/Dev/assimp-6.0.4")
    include_directories(
            ${PROJECT_SOURCE_DIR}${SDL3_PATH}/include
            ${PROJECT_SOURCE_DIR}${SDL3_IMAGE_PATH}/include
            ${PROJECT_SOURCE_DIR}/src
            ${PROJECT_SOURCE_DIR}/src/utils
            ${PROJECT_SOURCE_DIR}/src/ecs
            ${PROJECT_SOURCE_DIR}/src/ecs/system
            ${PROJECT_SOURCE_DIR}/src/vendor
            ${PROJECT_SOURCE_DIR}/src/vendor/glad/include
            ${PROJECT_SOURCE_DIR}/src/renderer
            ${PROJECT_SOURCE_DIR}${ASSIMP_PATH}/include
            ${PROJECT_SOURCE_DIR}${ASSIMP_PATH}/build/include
    )
    link_directories(
            ${PROJECT_SOURCE_DIR}${SDL3_PATH}/lib
            ${PROJECT_SOURCE_DIR}${SDL3_IMAGE_PATH}/lib
            ${PROJECT_SOURCE_DIR}${ASSIMP_PATH}/build/lib
    )
endif ()

add_executable(echoes_of_the_forgotten_keep src/main.cpp
        src/Game.cpp
        src/TextureManager.cpp
        src/Map.cpp
        src/GameObject.cpp
        src/utils/Vector2D.cpp
        src/utils/Collision.cpp
        src/ecs/system/CollisionSystem.cpp
        src/ecs/World.cpp
        src/vendor/tinyxml2.cpp
        src/manager/AssetManager.cpp
        src/manager/Scene.cpp
        src/vendor/glad/glad.c
        src/renderer/Shader.cpp
        src/renderer/Animation3D.cpp
        src/renderer/modelManager.cpp
        src/renderer/AnimationManager.cpp
)

target_link_libraries(echoes_of_the_forgotten_keep
        SDL3
        SDL3_image
        opengl32
        assimp
        ${PROJECT_SOURCE_DIR}${ASSIMP_PATH}/build/lib/libassimp.dll.a
)

