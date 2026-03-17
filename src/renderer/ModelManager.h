//
// Created by adamd on 2026-03-11.
//

#ifndef ECHOESOFTHEFORGOTTENKEEP_MODELMANAGER_H
#define ECHOESOFTHEFORGOTTENKEEP_MODELMANAGER_H

#include "Mesh.h"
#include "Component.h"
#include "assimp/scene.h"


class ModelManager {
public:
    static std::unordered_map<std::string, Model*> models;
    static Model* load(const std::string& modelPath);
    static void SetVertexBoneData(Vertex& vertex, int boneID, float weight);
    static void PrintSkeletonHierarchy(aiNode* node,
                            const std::unordered_map<std::string, int>& boneMap,
                            int depth);
    static int BuildRuntimeSkeleton(aiNode* node,
                                       Model& model,
                                       int parentIndex = -1);
};

#endif //ECHOESOFTHEFORGOTTENKEEP_MODELMANAGER_H