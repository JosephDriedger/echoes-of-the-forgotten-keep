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
        return it->second;
    }

    std::cout << "Loading model: " << modelPath << std::endl;
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
        delete model;
        return nullptr;
    }

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
        model->meshes->emplace_back(vertices, indices);
    }

    BuildRuntimeSkeleton(scene->mRootNode, *model, -1);

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

int ModelManager::BuildRuntimeSkeleton(aiNode* node,
                                       Model& model,
                                       int parentIndex)
{
    BoneNode bn;

    bn.name = node->mName.C_Str();

    // Bone ID mapping
    auto it = model.boneMap.find(bn.name);
    bn.boneID = (it != model.boneMap.end()) ? it->second : -1;

    // 🔥 store parent instead of children
    bn.parentIndex = parentIndex;

    // Transform
    aiMatrix4x4 t = node->mTransformation;
    bn.localTransform = glm::mat4(
        t.a1, t.b1, t.c1, t.d1,
        t.a2, t.b2, t.c2, t.d2,
        t.a3, t.b3, t.c3, t.d3,
        t.a4, t.b4, t.c4, t.d4
    );

    // Push node
    int index = (int)model.skeleton.size();
    model.skeleton.push_back(bn);

    // 🔥 recurse children AFTER pushing (ensures parent comes first)
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        BuildRuntimeSkeleton(node->mChildren[i], model, index);
    }

    return index;
}