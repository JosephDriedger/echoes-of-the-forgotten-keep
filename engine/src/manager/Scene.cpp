//
// Created by adamd on 2026-02-25.
//

#include "Scene.h"

#include "AnimationManager.h"
#include "ModelManager.h"

Scene::Scene (SceneType sceneType, const char* sceneName, const char* mapPath, int windowWidth, int windowHeight) : name(sceneName) {

    auto& camera = world.createEntity();
    camera.addComponent<Transform3D>(glm::vec3(0,1,5)); // back a bit
    camera.addComponent<Camera3D>();

    createPlayer();
    createEnemy();

    // add scene state
    auto& state (world.createEntity());
    state.addComponent<SceneState>();
}

void Scene::createPlayer() {
    auto& player(world.createEntity());
    auto& playerTransform = player.addComponent<Transform3D>(glm::vec3(-2,0,0));
    player.addComponent<Model>(ModelManager::load("../asset/Knight.glb"));
    player.addComponent<Texture3D>(*TextureManager::load3D("../asset/knight_texture.png"));
    auto& anim3D = player.addComponent<Animation3D>(AnimationManager::load("../asset/animations/Rig_Medium_General.glb"));
    AnimationManager::appendClips("../asset/animations/Rig_Medium_MovementBasic.glb", *anim3D.clips);
    AnimationManager::appendClips("../asset/animations/Rig_Medium_MovementAdvanced.glb", *anim3D.clips);
    AnimationManager::appendClips("../asset/animations/Rig_Medium_Simulation.glb", *anim3D.clips);
    AnimationManager::appendClips("../asset/animations/Rig_Medium_Special.glb", *anim3D.clips);
    AnimationManager::appendClips("../asset/animations/Rig_Medium_Tools.glb", *anim3D.clips);
    AnimationManager::appendClips("../asset/animations/Rig_Medium_CombatMelee.glb", *anim3D.clips);
    AnimationManager::appendClips("../asset/animations/Rig_Medium_CombatRanged.glb", *anim3D.clips);
    auto& animator = player.addComponent<Animator>();
    animator.currentClip = 7;

    auto& sword(world.createEntity());
    auto& swordTransform = sword.addComponent<Transform3D>(glm::vec3(-2,0,0));
    sword.addComponent<Model>(ModelManager::load("../asset/sword_1handed.gltf"));
    sword.addComponent<Texture3D>(*TextureManager::load3D("../asset/knight_texture.png"));
    sword.addComponent<BoneAttachment>(&player, "handslot.r", glm::mat4(1.0f));
    // sword.addComponent<ProjectileTag>(true, false);
    // sword.addComponent<Parent>().entity = &player;
    // sword.addComponent<Collider3D>(0.5f, 0.5f, 0.5f);
    // sword.addComponent<Damage>(1);

    auto& shield(world.createEntity());
    auto& shieldTransform = shield.addComponent<Transform3D>(glm::vec3(-2,0,0));
    shield.addComponent<Model>(ModelManager::load("../asset/shield_spikes.gltf"));
    shield.addComponent<Texture3D>(*TextureManager::load3D("../asset/knight_texture.png"));
    shield.addComponent<BoneAttachment>(&player, "handslot.l", glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.15f)));

    player.addComponent<PlayerTag>();
    player.addComponent<Velocity3D>(glm::vec3(0,0,0),3.0f);

    auto& playerCombat = player.addComponent<Combat>(Attack{0.0f, 0.0f}, 2.0f);
    player.addComponent<Health>(5);
    player.addComponent<Collider3D>(1, 7, 0.5);
    // playerCombat.weapon = &sword;

    // auto& player2(world.createEntity());
    // auto& playerTransform2 = player2.addComponent<Transform3D>(glm::vec3(2,0,2));
    // player2.addComponent<Model>(ModelManager::load("../asset/Rogue_Hooded.glb"));
    // player2.addComponent<Texture3D>(*TextureManager::load3D("../asset/rogue_texture.png"));
    // auto& anim3D2 = player2.addComponent<Animation3D>(AnimationManager::load("../asset/animations/Rig_Medium_General.glb"));
    // auto& animator2 = player2.addComponent<Animator>();
    // animator2.currentClip = 7;

    auto& wall(world.createEntity());
    auto& wallTransform = wall.addComponent<Transform3D>(glm::vec3(-2,0,-1));
    wall.addComponent<Model>(ModelManager::load("../asset/dungeon/wall.gltf"));
    wall.addComponent<Texture3D>(*TextureManager::load3D("../asset/dungeon/dungeon_texture.png"));

    auto& doorway(world.createEntity());
    auto& doorwayTransform = doorway.addComponent<Transform3D>(glm::vec3(2,0,-1));
    doorway.addComponent<Model>(ModelManager::load("../asset/dungeon/wall_doorway_scaffold.gltf"));
    doorway.addComponent<Texture3D>(*TextureManager::load3D("../asset/dungeon/dungeon_texture.png"));

    auto& door(world.createEntity());
    auto& doorTransform = door.addComponent<Transform3D>(glm::vec3(1.18,0,-1),glm::vec3(0,60,0));
    door.addComponent<Model>(ModelManager::load("../asset/dungeon/door.gltf"));
    door.addComponent<Texture3D>(*TextureManager::load3D("../asset/dungeon/dungeon_texture.png"));

    auto& tile(world.createEntity());
    auto& tileTransform = tile.addComponent<Transform3D>(glm::vec3(0,-.1,1));
    tile.addComponent<Model>(ModelManager::load("../asset/dungeon/floor_tile_large.gltf"));
    tile.addComponent<Texture3D>(*TextureManager::load3D("../asset/dungeon/dungeon_texture.png"));

    auto& tile2(world.createEntity());
    auto& tileTransform2 = tile2.addComponent<Transform3D>(glm::vec3(2,-.1,1));
    tile2.addComponent<Model>(ModelManager::load("../asset/dungeon/floor_tile_large.gltf"));
    tile2.addComponent<Texture3D>(*TextureManager::load3D("../asset/dungeon/dungeon_texture.png"));
}

void Scene::createEnemy() {
    auto& enemy(world.createEntity());
    auto& enemyTransform = enemy.addComponent<Transform3D>(glm::vec3(6,0,2));

    enemy.addComponent<Velocity3D>(glm::vec3(0,0,0), 2.5f);

    enemy.addComponent<AI>(AI{
        AIState::Idle,     // start idle
        0.0f,              // state timer
        7.0f,            // vision range
        nullptr            // target (Entity*)
    });

    enemy.addComponent<Patrol>(Patrol{
        glm::vec3{6, 0, 2},  // point A
        glm::vec3{40, 0, 2},  // point B
        true                  // movingToB
    });

    enemy.addComponent<Model>(ModelManager::load("../asset/Rogue_Hooded.glb"));
    enemy.addComponent<Texture3D>(*TextureManager::load3D("../asset/rogue_texture.png"));
    enemy.addComponent<EnemyTag>();
    auto& anim3D2 = enemy.addComponent<Animation3D>(AnimationManager::load("../asset/animations/Rig_Medium_General.glb"));
    auto& animator2 = enemy.addComponent<Animator>();
    animator2.currentClip = 7;

    auto& enemyCombat = enemy.addComponent<Combat>(Attack{2.0f, 0.0f}, 2.0f);
    enemy.addComponent<Health>(3);
    enemy.addComponent<Collider3D>(1, 7, 0.5);

    auto& sword(world.createEntity());
    auto& swordTransform = sword.addComponent<Transform3D>(glm::vec3(-2,0,0));
    sword.addComponent<Model>(ModelManager::load("../asset/sword_1handed.gltf"));
    sword.addComponent<Texture3D>(*TextureManager::load3D("../asset/knight_texture.png"));
    sword.addComponent<BoneAttachment>(&enemy, "handslot.r", glm::mat4(1.0f));
    // sword.addComponent<ProjectileTag>(true, false);
    // sword.addComponent<Parent>().entity = &enemy;
    // sword.addComponent<Collider3D>(0.5f, 0.5f, 0.5f);
    // sword.addComponent<Damage>(1);
    // enemyCombat.weapon = &sword;

}