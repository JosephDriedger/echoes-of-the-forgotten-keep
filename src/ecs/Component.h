//
// Created by adamd on 2026-01-21.
//

#ifndef ASSIGNMENT1_COMPONENT_H
#define ASSIGNMENT1_COMPONENT_H
#include <functional>
#include <iostream>
#include <SDL3/SDL_render.h>
#include <string>
#include <unordered_map>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <SDL3_image/SDL_image.h>
#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>
#include <vendor/glm/gtc/type_ptr.hpp>
#include "Mesh.h"

#include "AinmationClip.h"
#include "TextureManager.h"
#include "Vector2D.h"

struct Transform {
    Vector2D position{};
    float rotation{};
    float scale{};
    Vector2D oldPosition{};
};

// direction and speed
struct Velocity {
    Vector2D direction{};
    float speed{};
};

struct Sprite {
    SDL_Texture *texture = nullptr;
    SDL_FRect src{};
    SDL_FRect dst{};
};

struct Collider {
    std::string tag;
    SDL_FRect rect{};
};

struct Animation {
    std::unordered_map<std::string, AnimationClip> clips{};
    std::string currentClip{};
    float time{}; // time accumulated for the current frame in the clip
    int currentFrame{}; // index of the current frame in the clip
    float speed = 0.1f; // time per frame
    int lastDirection = 2;
};

struct Camera {
    SDL_FRect view;
    float worldWidth;
    float worldHeight;
};

struct TimedSpawner {
    float spawnInterval {};
    std::function<void()> spawnCallback {};
    float timer {};
};

// our game state, might have multiple scenes
struct SceneState {
    int coinsCollected = 0;
};

// 3D Components

struct MeshRenderer {
    GLuint vao = 0;
    GLuint texture = 0;
    int vertexCount = 0;
};

struct SkeletonComponent {
    //std::shared_ptr<Skeleton> skeleton;
    std::vector<glm::mat4> boneMatrices; // matrices for shader skinning
};

struct Velocity3D {
    glm::vec3 direction{0.0f};
    float speed{0.0f};
};

struct Collider3D {
    std::string tag;
    glm::vec3 size{1.0f, 1.0f, 1.0f};
    glm::vec3 offset{0.0f};
};

// Transform3D tested
struct Transform3D {
    glm::vec3 position;
    glm::vec3 rotation; // pitch, yaw, roll in radians
    glm::vec3 scale;

    glm::vec3 oldPosition;

    Transform3D(glm::vec3 pos = glm::vec3(0.0f),
                glm::vec3 rot = glm::vec3(0.0f),
                glm::vec3 scl = glm::vec3(1.0f))
        : position(pos), rotation(rot), scale(scl) {}
};

// Camera3D part tested
struct Camera3D {
    glm::vec3 forward = glm::vec3(0,0,-1);
    glm::vec3 up      = glm::vec3(0,1,0);
    float fov         = 45.0f;
    float aspectRatio = 800.0f / 600.0f;
    float nearPlane   = 0.1f;
    float farPlane    = 100.0f;
};

// Model tested
struct Model {
    std::string path;      // path to GLB/GLTF
    std::vector<Mesh> meshes;

    Model(const std::string& modelPath) : path(modelPath) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            path,
            aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
        );

        if (!scene || !scene->HasMeshes()) {
            std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
            return;
        }

        std::cout << "Model loaded successfully: " << path << "\n";
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

            meshes.push_back(Mesh(vertices, indices));
        }
    }
};

// Texture3D tested
struct Texture3D {
    GLuint id;
    std::string path;

    Texture3D(const char* texPath) : path(texPath) {
        id = TextureManager::load3D(texPath);
    }
};

// Animation3D
struct Animation3D {
    std::string path;
    // store animation data loaded via Assimp (e.g., aiAnimation pointers)
    const aiScene* scene = nullptr;

    Animation3D(const std::string& animPath) : path(animPath) {
        Assimp::Importer importer;
        scene = importer.ReadFile(animPath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if (!scene) {
            std::cerr << "Failed to load animation: " << importer.GetErrorString() << std::endl;
        }
    }
};

struct PlayerTag{};
struct ProjectileTag {};

#endif //ASSIGNMENT1_COMPONENT_H