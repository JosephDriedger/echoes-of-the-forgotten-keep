//
// Created by Joseph Driedger on 3/17/2026.
//

#include <iostream>

int RunECSTests();
int RunSceneManagerTests();
int RunCollisionTests();
int RunRoomTransitionTests();
int RunEnemyAITests();

int main()
{
    RunECSTests();
    RunSceneManagerTests();
    RunCollisionTests();
    RunRoomTransitionTests();
    RunEnemyAITests();

    std::cout << "All tests passed.\n";
    return 0;
}