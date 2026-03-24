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
            ${PROJECT_SOURCE_DIR}/engine/src
            ${PROJECT_SOURCE_DIR}/engine/src/utils
            ${PROJECT_SOURCE_DIR}/src/ecs
            ${PROJECT_SOURCE_DIR}/engine/src/ecs/system
            ${PROJECT_SOURCE_DIR}/engine/src/ecs
            ${PROJECT_SOURCE_DIR}/engine/src/manager
            ${PROJECT_SOURCE_DIR}/engine/src/rendering
            ${PROJECT_SOURCE_DIR}/engine/src/vendor
            ${PROJECT_SOURCE_DIR}/engine/src/vendor/glad/include
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

add_executable(echoes_of_the_forgotten_keep engine/src/main.cpp
        engine/src/Game.cpp
        engine/src/manager/TextureManager.cpp
        engine/src/Map.cpp
        engine/src/GameObject.cpp
        engine/src/utils/Vector2D.cpp
        engine/src/utils/Collision.cpp
        engine/src/ecs/system/CollisionSystem.cpp
        engine/src/ecs/World.cpp
        engine/src/vendor/tinyxml2.cpp
        engine/src/manager/Scene.cpp
        engine/src/vendor/glad/glad.c
        engine/src/rendering/Shader.cpp
        engine/src/rendering/Animation3D.cpp
        engine/src/rendering/ModelManager.cpp
        engine/src/rendering/AnimationManager.cpp
        engine/src/ecs/system/EventResponseSystem.cpp
        engine/src/ecs/system/MainMenuSystem.cpp
        engine/src/ecs/system/EnemyAISystem.cpp
        engine/src/ecs/system/DamageSystem.cpp
        engine/src/ecs/system/CombatSystem.cpp
        engine/src/ecs/system/MouseInputSystem.cpp
#add_executable(echoes_of_the_forgotten_keep src/main.cpp
#        src/Game.cpp
#        src/TextureManager.cpp
#        src/Map.cpp
#        src/GameObject.cpp
#        src/utils/Vector2D.cpp
#        src/utils/Collision.cpp
#        src/ecs/system/CollisionSystem.cpp
#        src/ecs/World.cpp
#        src/vendor/tinyxml2.cpp
#        src/manager/AssetManager.cpp
#        src/manager/Scene.cpp
#        src/vendor/glad/glad.c
#        src/renderer/Shader.cpp
#        src/renderer/Animation3D.cpp
#        src/renderer/modelManager.cpp
#        src/renderer/AnimationManager.cpp
#        src/ecs/system/EventResponseSystem.cpp
#        src/ecs/system/MainMenuSystem.cpp
#        src/ecs/system/EnemyAISystem.cpp
#        src/ecs/system/DamageSystem.cpp
#        src/ecs/system/CombatSystem.cpp
#        src/ecs/system/MouseInputSystem.cpp
)

target_link_libraries(echoes_of_the_forgotten_keep
        SDL3
        SDL3_image
        opengl32
        assimp
        ${PROJECT_SOURCE_DIR}${ASSIMP_PATH}/build/lib/libassimp.dll.a
)

