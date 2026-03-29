//
// Created by Joseph Driedger on 3/17/2026.
//

#include <iostream>

// Engine tests
int RunECSTests();
int RunSceneManagerTests();
int RunAnimatorTests();

// Game tests
int RunCollisionTests();
int RunRoomTransitionTests();
int RunEntitySpawnSystemTests();
int RunAnimationSystemTests();
int RunBoneAttachmentTests();

int main()
{
    RunECSTests();
    RunSceneManagerTests();
    RunAnimatorTests();

    RunCollisionTests();
    RunRoomTransitionTests();
    RunEntitySpawnSystemTests();
    RunAnimationSystemTests();
    RunBoneAttachmentTests();

    std::cout << "All tests passed.\n";
    return 0;
}
