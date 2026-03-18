//
// Created by adamd on 2026-03-11.
//

#include "ModelManager.h"

#include <iostream>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <SDL3_image/SDL_image.h>
#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>
#include <vendor/glm/gtc/type_ptr.hpp>
#include <string>

#include "assimp/Importer.hpp"

std::unordered_map<std::string, Model*> ModelManager::models;

Model* ModelManager::load(const std::string& modelPath) {

    auto it = models.find(modelPath);
    if (it != models.end()) {
        std::cout << "Model loaded successfully from modelMeshes: " << modelPath << "\n";
        return it->second;
    }

    Assimp::Importer importer;
    auto* model = new Model();
    model->meshes = new std::vector<Mesh>();
    const aiScene* scene = importer.ReadFile(
        modelPath,
        aiProcess_Triangulate |
        aiProcess_FlipUVs |
        aiProcess_CalcTangentSpace |
        aiProcess_LimitBoneWeights
    );

    if (!scene || !scene->HasMeshes()) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }

    std::cout << "Model loaded successfully: " << modelPath << "\n";
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
        std::cout << "Mesh " << i << ": " << scene->mMeshes[i]->mName.C_Str() << std::endl;

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {

            Vertex vertex{};

            // Initialize bone data
            for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
                vertex.BoneIDs[i] = -1;
                vertex.Weights[i] = 0.0f;
            }

            vertex.Position = glm::vec3(
                mesh->mVertices[v].x,
                mesh->mVertices[v].y,
                mesh->mVertices[v].z
            );

            if (mesh->HasNormals())
                vertex.Normal = glm::vec3(
                    mesh->mNormals[v].x,
                    mesh->mNormals[v].y,
                    mesh->mNormals[v].z
                );

            if (mesh->mTextureCoords[0])
                vertex.TexCoords = glm::vec2(
                    mesh->mTextureCoords[0][v].x,
                    mesh->mTextureCoords[0][v].y
                );

            vertices.push_back(vertex);
        }

        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        if (mesh->HasBones()) {

            for (unsigned int boneIndex = 0; boneIndex < mesh->mNumBones; boneIndex++) {

                aiBone* aiBonePtr = mesh->mBones[boneIndex];
                std::string boneName = aiBonePtr->mName.C_Str();

                int boneID = -1;

                if (model->boneMap.find(boneName) == model->boneMap.end()) {

                    BoneInfo newBone;

                    newBone.id = model->boneCounter;

                    aiMatrix4x4 offset = aiBonePtr->mOffsetMatrix;

                    newBone.offset = glm::mat4(
                        offset.a1, offset.b1, offset.c1, offset.d1,
                        offset.a2, offset.b2, offset.c2, offset.d2,
                        offset.a3, offset.b3, offset.c3, offset.d3,
                        offset.a4, offset.b4, offset.c4, offset.d4
                    );

                    model->boneInfo.push_back(newBone);

                    boneID = model->boneCounter;
                    model->boneMap[boneName] = model->boneCounter;

                    model->boneCounter++;
                }
                else {
                    boneID = model->boneMap[boneName];
                }

                for (unsigned int weightIndex = 0; weightIndex < aiBonePtr->mNumWeights; weightIndex++) {

                    int vertexId = aiBonePtr->mWeights[weightIndex].mVertexId;
                    float weight = aiBonePtr->mWeights[weightIndex].mWeight;

                    if (vertexId >= vertices.size())
                        continue;

                    SetVertexBoneData(vertices[vertexId], boneID, weight);
                }
            }
        }
        else {
            std::cout << "Mesh " << i << " has no bones\n";
        }

        model->meshes->emplace_back(vertices, indices);
    }


    std::cout << "Scene animations: " << scene->mNumAnimations << "\n";

    for (unsigned int a = 0; a < scene->mNumAnimations; a++) {

        aiAnimation* anim = scene->mAnimations[a];

        std::cout << "\nAnimation " << a << "\n";
        std::cout << "--------------------------------\n";
        std::cout << "Name: " << anim->mName.C_Str() << "\n";
        std::cout << "Duration: " << anim->mDuration << "\n";
        std::cout << "Ticks per second: " << anim->mTicksPerSecond << "\n";
        std::cout << "Channels (bones): " << anim->mNumChannels << "\n";

        // for (unsigned int c = 0; c < anim->mNumChannels; c++) {
        //
        //     aiNodeAnim* channel = anim->mChannels[c];
        //
        //     std::string boneName = channel->mNodeName.C_Str();
        //
        //     std::cout << "\n  Bone " << c << ": " << boneName << "\n";
        //
        //     std::cout << "    Position keys: " << channel->mNumPositionKeys << "\n";
        //     std::cout << "    Rotation keys: " << channel->mNumRotationKeys << "\n";
        //     std::cout << "    Scale keys: " << channel->mNumScalingKeys << "\n";
        //
        //     if (channel->mNumPositionKeys > 0) {
        //         std::cout << "    First position key time: "
        //                   << channel->mPositionKeys[0].mTime << "\n";
        //     }
        //
        //     if (channel->mNumRotationKeys > 0) {
        //         std::cout << "    First rotation key time: "
        //                   << channel->mRotationKeys[0].mTime << "\n";
        //     }
        // }
    }

    std::cout << "\n========== SKELETON HIERARCHY ==========\n";

    // model->rootBoneName = scene->mRootNode->mName.C_Str();
    int index = BuildRuntimeSkeleton(scene->mRootNode, *model, -1);

    // Find hips node in skeleton
    for (int i = 0; i < model->skeleton.size(); i++) {
        if (model->skeleton[i].name == "hips") {
            model->rootBoneIndex = i;
            break;
        }
    }
    std::cout << "Skeleton index: " << model->rootBoneIndex << "\n";
    // If not found, fallback to first node
    if (model->rootBoneIndex == -1)
        model->rootBoneIndex = 0;

    PrintSkeletonHierarchy(scene->mRootNode, model->boneMap, 0);

    PrintAllNodes(scene->mRootNode);
    models[modelPath] = model;

    return model;
}

void ModelManager::SetVertexBoneData(Vertex& vertex, int boneID, float weight)
{
    for (int i = 0; i < MAX_BONE_INFLUENCE; i++)
    {
        if (vertex.BoneIDs[i] < 0)
        {
            vertex.BoneIDs[i] = boneID;
            vertex.Weights[i] = weight;
            return;
        }
    }
}

void ModelManager::PrintSkeletonHierarchy(aiNode* node,
                            const std::unordered_map<std::string, int>& boneMap,
                            int depth = 0)
{
    std::string nodeName = node->mName.C_Str();

    bool isBone = boneMap.find(nodeName) != boneMap.end();

    if (isBone)
    {
        for (int i = 0; i < depth; i++)
            std::cout << "  ";

        std::cout << nodeName
                  << " (Bone ID: " << boneMap.at(nodeName) << ")"
                  << std::endl;
    }

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        PrintSkeletonHierarchy(node->mChildren[i], boneMap, depth + 1);
    }
}

void ModelManager::PrintAllNodes(aiNode* node, int depth)
{
    for (int i = 0; i < depth; i++)
        std::cout << "  ";

    std::cout << node->mName.C_Str() << "\n";

    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        PrintAllNodes(node->mChildren[i], depth + 1);
    }
}

int ModelManager::BuildRuntimeSkeleton(aiNode* node,
                                       Model& model,
                                       int parentIndex)
{
    BoneNode bn;
    bn.name = node->mName.C_Str();
    bn.boneID = model.boneMap.count(bn.name) ? model.boneMap[bn.name] : -1;

    aiMatrix4x4 t = node->mTransformation;
    bn.localTransform = glm::mat4(
        t.a1, t.b1, t.c1, t.d1,
        t.a2, t.b2, t.c2, t.d2,
        t.a3, t.b3, t.c3, t.d3,
        t.a4, t.b4, t.c4, t.d4
    );

    int index = (int)model.skeleton.size();
    model.skeleton.push_back(bn);

    if (parentIndex >= 0)
        model.skeleton[parentIndex].children.push_back(index);

    // Recurse children
    for (unsigned int i = 0; i < node->mNumChildren; i++)
        BuildRuntimeSkeleton(node->mChildren[i], model, index);

    return index;
}