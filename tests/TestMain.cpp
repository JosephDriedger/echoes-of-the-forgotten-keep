//
// Created by Joseph Driedger on 3/17/2026.
//

#include <iostream>

int RunECSTests();
int RunSceneManagerTests();

int main()
{
    RunECSTests();
    RunSceneManagerTests();

    std::cout << "All tests passed.\n";
    return 0;
}