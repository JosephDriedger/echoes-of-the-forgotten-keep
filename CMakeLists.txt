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
            ${PROJECT_SOURCE_DIR}/playground/src
            ${PROJECT_SOURCE_DIR}/playground/src/utils
            ${PROJECT_SOURCE_DIR}/playground/src/ecs/system
            ${PROJECT_SOURCE_DIR}/playground/src/ecs
            ${PROJECT_SOURCE_DIR}/playground/src/manager
            ${PROJECT_SOURCE_DIR}/playground/src/rendering
            ${PROJECT_SOURCE_DIR}/playground/src/vendor
            ${PROJECT_SOURCE_DIR}/playground/src/vendor/glad/include
            ${PROJECT_SOURCE_DIR}/src/ecs
            ${PROJECT_SOURCE_DIR}${ASSIMP_PATH}/include
            ${PROJECT_SOURCE_DIR}${ASSIMP_PATH}/build/include
    )
    link_directories(
            ${PROJECT_SOURCE_DIR}${SDL3_PATH}/lib
            ${PROJECT_SOURCE_DIR}${SDL3_IMAGE_PATH}/lib
            ${PROJECT_SOURCE_DIR}${ASSIMP_PATH}/build/lib
    )
endif ()

add_executable(echoes_of_the_forgotten_keep playground/src/main.cpp
        playground/src/Game.cpp
        playground/src/manager/TextureManager.cpp
        playground/src/Map.cpp
        playground/src/GameObject.cpp
        playground/src/utils/Vector2D.cpp
        playground/src/utils/Collision.cpp
        playground/src/ecs/system/CollisionSystem.cpp
        playground/src/ecs/World.cpp
        playground/src/vendor/tinyxml2.cpp
        playground/src/manager/Scene.cpp
        playground/src/vendor/glad/glad.c
        playground/src/rendering/Shader.cpp
        playground/src/rendering/Animation3D.cpp
        playground/src/rendering/ModelManager.cpp
        playground/src/rendering/AnimationManager.cpp
        playground/src/ecs/system/SpawnSystem.cpp
        playground/src/ecs/system/DamageSystem.cpp
        playground/src/ecs/system/CombatSystem.cpp
        playground/src/ecs/system/DebugRenderSystem.cpp
        playground/src/ecs/system/EnemyAISystem.cpp
        playground/src/ecs/system/MainMenuSystem.cpp
        playground/src/ecs/system/MouseInputSystem.cpp
        playground/src/ecs/system/EventResponseSystem.cpp
        playground/src/ecs/system/BuildRoomSystem.cpp
        playground/src/ecs/system/DoorPuzzleSystem.cpp
)

target_link_libraries(echoes_of_the_forgotten_keep
        SDL3
        SDL3_image
        opengl32
        assimp
        ${PROJECT_SOURCE_DIR}${ASSIMP_PATH}/build/lib/libassimp.dll.a
)