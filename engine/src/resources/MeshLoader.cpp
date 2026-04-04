// Created by Joey Driedger

#include "engine/resources/MeshLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>

namespace engine
{
    namespace
    {
        void SetVertexBoneData(Vertex& vertex, int boneID, float weight)
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

        int BuildRuntimeSkeleton(aiNode* node, Skeleton& skeleton, int parentIndex = -1)
        {
            BoneNode bn;
            bn.Name = node->mName.C_Str();

            auto it = skeleton.BoneMap.find(bn.Name);
            bn.BoneId = (it != skeleton.BoneMap.end()) ? it->second : -1;
            bn.ParentIndex = parentIndex;

            aiMatrix4x4 t = node->mTransformation;
            bn.LocalTransform = glm::mat4(
                t.a1, t.b1, t.c1, t.d1,
                t.a2, t.b2, t.c2, t.d2,
                t.a3, t.b3, t.c3, t.d3,
                t.a4, t.b4, t.c4, t.d4
            );

            int index = static_cast<int>(skeleton.Nodes.size());
            skeleton.Nodes.push_back(bn);

            for (unsigned int i = 0; i < node->mNumChildren; i++)
            {
                BuildRuntimeSkeleton(node->mChildren[i], skeleton, index);
            }

            return index;
        }
    }

    std::shared_ptr<Mesh> MeshLoader::LoadQuad() const
    {
        std::vector<Vertex> vertices =
        {
            {-0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 0.0f, {-1,-1,-1,-1}, {0,0,0,0}},
            { 0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 0.0f, {-1,-1,-1,-1}, {0,0,0,0}},
            { 0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f, {-1,-1,-1,-1}, {0,0,0,0}},
            {-0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f, {-1,-1,-1,-1}, {0,0,0,0}}
        };

        std::vector<unsigned int> indices = {0, 1, 2, 2, 3, 0};

        auto mesh = std::make_shared<Mesh>();
        if (!mesh->Create(vertices, indices))
        {
            return nullptr;
        }
        return mesh;
    }

    MeshLoader::Result MeshLoader::LoadFromFile(const std::string& path) const
    {
        Result result;

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            path,
            aiProcess_Triangulate |
            aiProcess_FlipUVs |
            aiProcess_CalcTangentSpace |
            aiProcess_LimitBoneWeights
        );

        if (!scene || !scene->HasMeshes())
        {
            std::cerr << "[MeshLoader] Failed to load: " << path << " - " << importer.GetErrorString() << '\n';
            return result;
        }

        auto skeleton = std::make_shared<Skeleton>();
        std::vector<Vertex> allVertices;
        std::vector<unsigned int> allIndices;

        for (unsigned int m = 0; m < scene->mNumMeshes; m++)
        {
            aiMesh* aiMeshPtr = scene->mMeshes[m];
            unsigned int baseVertex = static_cast<unsigned int>(allVertices.size());

            for (unsigned int v = 0; v < aiMeshPtr->mNumVertices; v++)
            {
                Vertex vertex{};

                vertex.PositionX = aiMeshPtr->mVertices[v].x;
                vertex.PositionY = aiMeshPtr->mVertices[v].y;
                vertex.PositionZ = aiMeshPtr->mVertices[v].z;

                if (aiMeshPtr->HasNormals())
                {
                    vertex.NormalX = aiMeshPtr->mNormals[v].x;
                    vertex.NormalY = aiMeshPtr->mNormals[v].y;
                    vertex.NormalZ = aiMeshPtr->mNormals[v].z;
                }

                if (aiMeshPtr->mTextureCoords[0])
                {
                    vertex.TextureU = aiMeshPtr->mTextureCoords[0][v].x;
                    vertex.TextureV = aiMeshPtr->mTextureCoords[0][v].y;
                }

                allVertices.push_back(vertex);
            }

            for (unsigned int f = 0; f < aiMeshPtr->mNumFaces; f++)
            {
                aiFace& face = aiMeshPtr->mFaces[f];
                for (unsigned int j = 0; j < face.mNumIndices; j++)
                {
                    allIndices.push_back(baseVertex + face.mIndices[j]);
                }
            }

            if (aiMeshPtr->HasBones())
            {
                for (unsigned int b = 0; b < aiMeshPtr->mNumBones; b++)
                {
                    aiBone* aiBonePtr = aiMeshPtr->mBones[b];
                    std::string boneName = aiBonePtr->mName.C_Str();

                    int boneID = -1;

                    if (skeleton->BoneMap.find(boneName) == skeleton->BoneMap.end())
                    {
                        BoneInfo newBone;
                        newBone.Id = skeleton->BoneCount;

                        aiMatrix4x4 offset = aiBonePtr->mOffsetMatrix;
                        newBone.Offset = glm::mat4(
                            offset.a1, offset.b1, offset.c1, offset.d1,
                            offset.a2, offset.b2, offset.c2, offset.d2,
                            offset.a3, offset.b3, offset.c3, offset.d3,
                            offset.a4, offset.b4, offset.c4, offset.d4
                        );

                        skeleton->Bones.push_back(newBone);
                        boneID = skeleton->BoneCount;
                        skeleton->BoneMap[boneName] = skeleton->BoneCount;
                        skeleton->BoneCount++;
                    }
                    else
                    {
                        boneID = skeleton->BoneMap[boneName];
                    }

                    for (unsigned int w = 0; w < aiBonePtr->mNumWeights; w++)
                    {
                        unsigned int vertexId = baseVertex + aiBonePtr->mWeights[w].mVertexId;
                        float weight = aiBonePtr->mWeights[w].mWeight;

                        if (vertexId < allVertices.size())
                        {
                            SetVertexBoneData(allVertices[vertexId], boneID, weight);
                        }
                    }
                }
            }
        }

        BuildRuntimeSkeleton(scene->mRootNode, *skeleton);

        auto mesh = std::make_shared<Mesh>();
        if (!mesh->Create(allVertices, allIndices))
        {
            std::cerr << "[MeshLoader] Failed to create mesh from: " << path << '\n';
            return result;
        }

        result.MeshPtr = mesh;
        result.SkeletonPtr = skeleton;
        return result;
    }
}
