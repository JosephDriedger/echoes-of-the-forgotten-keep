//
// Created by adamd on 2026-03-11.
//

#ifndef ECHOESOFTHEFORGOTTENKEEP_MODELMANAGER_H
#define ECHOESOFTHEFORGOTTENKEEP_MODELMANAGER_H

#include "Mesh.h"


class ModelManager {
public:
    static std::unordered_map<std::string, std::vector<Mesh>*> modelMeshes;
    static std::vector<Mesh>* load(const std::string modelPath);

};



#endif //ECHOESOFTHEFORGOTTENKEEP_MODELMANAGER_H