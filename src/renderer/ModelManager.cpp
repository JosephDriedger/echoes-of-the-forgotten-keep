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

std::unordered_map<std::string, std::vector<Mesh>*> ModelManager::modelMeshes;

std::vector<Mesh>* ModelManager::load(const std::string modelPath) {

    auto it = modelMeshes.find(modelPath);
    if (it != modelMeshes.end()) {
        std::cout << "Model loaded successfully from modelMeshes: " << modelPath << "\n";
        return it->second;
    }


    Assimp::Importer importer;
    std::vector<Mesh>* meshes = new std::vector<Mesh>();
    const aiScene* scene = importer.ReadFile(
        modelPath,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
    );

    if (!scene || !scene->HasMeshes()) {
        std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;

    }

    std::cout << "Model loaded successfully: " << modelPath << "\n";
    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
        std::cout << "Mesh " << i << ": " << scene->mMeshes[i]->mName.C_Str() << std::endl;

    for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[i];
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int v = 0; v < mesh->mNumVertices; v++) {
            Vertex vertex;
            vertex.Position = glm::vec3(mesh->mVertices[v].x,
                                        mesh->mVertices[v].y,
                                        mesh->mVertices[v].z);
            if (mesh->HasNormals())
                vertex.Normal = glm::vec3(mesh->mNormals[v].x,
                                          mesh->mNormals[v].y,
                                          mesh->mNormals[v].z);
            if (mesh->mTextureCoords[0])
                vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][v].x,
                                             mesh->mTextureCoords[0][v].y);
            vertices.push_back(vertex);
        }

        for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
            aiFace face = mesh->mFaces[f];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        meshes->push_back(Mesh(vertices, indices));
    }

    modelMeshes[modelPath] = meshes;

    return meshes;
}
