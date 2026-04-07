//
// Created by Joseph Driedger on 3/17/2026.
//
// Test runner entry point for the Echoes of the Forgotten Keep engine and game tests.
// Each test suite uses assert() internally; any failure aborts the process.

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

// Runs all engine and game test suites sequentially.
// If all suites pass, prints a success message and exits with 0.
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
