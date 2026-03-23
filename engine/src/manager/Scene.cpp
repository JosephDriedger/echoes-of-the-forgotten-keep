//
// Created by adamd on 2026-02-25.
//

#include "Scene.h"

#include "../rendering/AnimationManager.h"
#include "../rendering/ModelManager.h"

Scene::Scene (const char* sceneName, const char* mapPath, int windowWidth, int windowHeight) : name(sceneName) {

    // load our map
    // world.getMap().load(mapPath, TextureManager::load("../asset/tileset.png"));

    //createMapColliders();

    // add entities
    //createItems();

    // Create Camera
    // auto& cam = world.createEntity();
    // SDL_FRect camView {};
    // camView.w = windowWidth;
    // camView.h = windowHeight;
    // cam.addComponent<Camera>(camView,
    //     world.getMap().width * world.getMap().tileWidth,
    //     world.getMap().height * world.getMap().tileHeight);

    auto& camera = world.createEntity();
    camera.addComponent<Transform3D>(glm::vec3(0,1,5)); // back a bit
    camera.addComponent<Camera3D>();

    createPlayer();

    //createSpawners();

    // add scene state
    auto& state (world.createEntity());
    state.addComponent<SceneState>();
}

void Scene::createMapColliders() {
    // for (auto &collider : world.getMap().colliders) {
    //     auto& e = world.createEntity();
    //     e.addComponent<Transform>(Vector2D(collider.rect.x, collider.rect.y), 0.0f,1.0f);
    //     auto& c = e.addComponent<Collider>("wall");
    //     c.rect.x = collider.rect.x;
    //     c.rect.y = collider.rect.y;
    //     c.rect.w = collider.rect.w;
    //     c.rect.h = collider.rect.h;
    // }
}

void Scene::createItems() {
    // for (auto &item : world.getMap().items) {
    //     auto& e = world.createEntity();
    //     e.addComponent<Transform>(Vector2D(item.rect.x, item.rect.y), 0.0f,1.0f);
    //     auto& c = e.addComponent<Collider>("item");
    //     c.rect.x = item.rect.x;
    //     c.rect.y = item.rect.y;
    //     c.rect.w = item.rect.w;
    //     c.rect.h = item.rect.h;
    //
    //     // just to have a visual of the colliders
    //     SDL_Texture* itemTex = TextureManager::load("../asset/coin.png");
    //     SDL_FRect itemSrc {0,0,32,32};
    //     SDL_FRect itemDst {c.rect.x,c.rect.y,c.rect.w, c.rect.h};
    //     e.addComponent<Sprite>(itemTex, itemSrc, itemDst);
    // }
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
    animator.currentClip = 0;

    auto& sword(world.createEntity());
    auto& swordTransform = sword.addComponent<Transform3D>(glm::vec3(-2,0,0));
    sword.addComponent<Model>(ModelManager::load("../asset/sword_1handed.gltf"));
    sword.addComponent<Texture3D>(*TextureManager::load3D("../asset/knight_texture.png"));
    sword.addComponent<BoneAttachment>(&player, "handslot.r", glm::mat4(1.0f));

    auto& shield(world.createEntity());
    auto& shieldTransform = shield.addComponent<Transform3D>(glm::vec3(-2,0,0));
    shield.addComponent<Model>(ModelManager::load("../asset/shield_spikes.gltf"));
    shield.addComponent<Texture3D>(*TextureManager::load3D("../asset/knight_texture.png"));
    shield.addComponent<BoneAttachment>(&player, "handslot.l", glm::mat4(1.0f));

    //player.addComponent<Animation3D>("../asset/Knight_anim.glb");

    //player.addComponent<Velocity>(Vector2D(0,0), 120.0f);

    // Animation anim = AssetManager::getAnimation("player");
    // player.addComponent<Animation>(anim);
    //
    // SDL_Texture* tex = TextureManager::load("../asset/animations/Swordsman_anim.png");
    // //SDL_FRect playerSrc {0,0,32,44};
    // SDL_FRect playerSrc = anim.clips[anim.currentClip].frameIndices[0];
    // SDL_FRect playerDst {playerTransform.position.x,playerTransform.position.y,128,128};
    // player.addComponent<Sprite>(tex, playerSrc, playerDst);
    // auto& playerCollider = player.addComponent<Collider>("player");
    //
    // // Shrink player collider for smoother movement
    // playerCollider.rect.w = playerDst.w/4;
    // playerCollider.rect.h = playerDst.h/4;

    player.addComponent<PlayerTag>();
    player.addComponent<Velocity3D>(glm::vec3(0,0,0),3.0f);

    auto& player2(world.createEntity());
    auto& playerTransform2 = player2.addComponent<Transform3D>(glm::vec3(2,0,-4));
    player2.addComponent<Model>(ModelManager::load("../asset/Rogue_Hooded.glb"));
    player2.addComponent<Texture3D>(*TextureManager::load3D("../asset/rogue_texture.png"));

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

void Scene::createSpawners() {
    // for (auto &spawner : world.getMap().spawner) {
    //     auto& entity = world.createEntity();
    //     Transform t = entity.addComponent<Transform>(Vector2D(spawner.rect.x, spawner.rect.y), 0.0f,1.0f);
    //     entity.addComponent<TimedSpawner>(2.0f, [this, t] {
    //         // create our projectile
    //         auto& e(world.createDeferredEntity());
    //         e.addComponent<Transform>(Vector2D(t.position.x,t.position.y),0.0f,1.0f);
    //         e.addComponent<Velocity>(Vector2D(0,-1),100.0f);
    //
    //         Animation anim = AssetManager::getAnimation("enemy");
    //         e.addComponent<Animation>(anim);
    //
    //         SDL_Texture* tex = TextureManager::load("../asset/animations/bird_anim.png");
    //         SDL_FRect src = {0,0,32,32};
    //         SDL_FRect dest {t.position.x,t.position.y,32,32};
    //         e.addComponent<Sprite>(tex, src, dest);
    //
    //         auto& c = e.addComponent<Collider>("projectile");
    //         c.rect.w = dest.w;
    //         c.rect.h = dest.h;
    //
    //
    //         e.addComponent<ProjectileTag>();
    //     });
    // }
}

// just to have a visual of the colliders
//SDL_Texture* tex = TextureManager::load("../asset/tileset.png");
//SDL_FRect colSrc {0,32,32,32};
//SDL_FRect colDst {c.rect.x,c.rect.y,c.rect.w, c.rect.h};
//e.addComponent<Sprite>(tex, colSrc, colDst);

// auto& item (world.createEntity());
// auto& itemTransform = item.addComponent<Transform>(Vector2D(500,96),0.0f,1.0f);
//
// SDL_Texture* itemTex = TextureManager::load("../asset/coin.png");
// SDL_FRect itemSrc{0,0,32,32};
//
// SDL_FRect itemDest {itemTransform.position.x,itemTransform.position.y,32,32};
// item.addComponent<Sprite>(itemTex,itemSrc, itemDest);
//
// auto& itemCollider = item.addComponent<Collider>("item");
// itemCollider.rect.w = itemDest.w;
// itemCollider.rect.h = itemDest.h;

//auto& spawner(world.createEntity());
//Transform t = spawner.addComponent<Transform>(Vector2D(windowWidth/2,windowHeight/2),0.0f,1.0f);
// spawner.addComponent<TimedSpawner>(2.0f, [this, t] {
//
//     // create our projectile
//     auto& e(world.createDeferredEntity());
//     e.addComponent<Transform>(Vector2D(t.position.x,t.position.y),0.0f,1.0f);
//     e.addComponent<Velocity>(Vector2D(0,-1),100.0f);
//
//     Animation anim = AssetManager::getAnimation("enemy");
//     e.addComponent<Animation>(anim);
//
//     SDL_Texture* tex = TextureManager::load("../asset/animations/bird_anim.png");
//     SDL_FRect src = {0,0,32,32};
//     SDL_FRect dest {t.position.x,t.position.y,32,32};
//     e.addComponent<Sprite>(tex, src, dest);
//
//     auto& c = e.addComponent<Collider>("projectile");
//     c.rect.w = dest.w;
//     c.rect.h = dest.h;
//
//
//     e.addComponent<ProjectileTag>();
// });