//
// Created by adamd on 2026-01-21.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERSYSTEM_H
#include <memory>
#include <vector>

#include "../Component.h"
#include "../Entity.h"
#include <iostream>

class RenderSystem {
public:
    Shader* shader3D;

    RenderSystem() {
        // Initialize 3D shader once
        shader3D = new Shader(
            "../playground/src/rendering/shaders/vertex.glsl",
            "../playground/src/rendering/shaders/fragment.glsl"
        );
        std::cout << "3D shader initialized in RenderSystem.\n";
        shader3D->use();
    }

    void render(const std::vector<std::unique_ptr<Entity>>& entities) {
        // Check that shader is ready
        if (!shader3D) {
            std::cout << "3D shader not initialized in RenderSystem.\n";
            return;
        }

        Entity* cameraEntity = nullptr;

        // Find camera
        for (auto& e : entities) {
            if (e->hasComponent<Camera3D>()) {
                cameraEntity = e.get();
                break;
            }
        }

        if (!cameraEntity) return; // no camera
        auto& cam = cameraEntity->getComponent<Camera3D>();

        for (auto& entity : entities) {
            if (entity->hasComponent<Model>() && entity->hasComponent<Transform3D>() && entity->hasComponent<Texture3D>()) {
                // TODO render meshes
                auto& model = entity->getComponent<Model>();
                auto& tex3D = entity->getComponent<Texture3D>();
                auto& transform = entity->getComponent<Transform3D>(); // for position, rotation, scale
                bool hasAnimator = entity->hasComponent<Animator>();

                // Bind shader
                //shader3D->use();

                // Compute matrices
                glm::mat4 modelMat;

                if (transform.useMatrix)
                {
                    modelMat = transform.modelMatrix;
                }
                else
                {
                    modelMat = glm::mat4(1.0f);
                    modelMat = glm::translate(modelMat, transform.position);
                    modelMat = glm::rotate(modelMat, glm::radians(transform.rotation.x), glm::vec3(1,0,0));
                    modelMat = glm::rotate(modelMat, glm::radians(transform.rotation.y), glm::vec3(0,1,0));
                    modelMat = glm::rotate(modelMat, glm::radians(transform.rotation.z), glm::vec3(0,0,1));
                    modelMat = glm::scale(modelMat, transform.scale);
                }

                // camera view & projection
                auto& camTrans = cameraEntity->getComponent<Transform3D>();
                glm::mat4 view = glm::lookAt(camTrans.position, camTrans.position + cam.forward, cam.up);
                glm::mat4 projection = glm::perspective(glm::radians(cam.fov), cam.aspectRatio, cam.nearPlane, cam.farPlane);

                // set uniforms
                glUniformMatrix4fv(glGetUniformLocation(shader3D->ID, "model"), 1, GL_FALSE, glm::value_ptr(modelMat));
                glUniformMatrix4fv(glGetUniformLocation(shader3D->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
                glUniformMatrix4fv(glGetUniformLocation(shader3D->ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

                // bind texture
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, tex3D.id);
                glUniform1i(glGetUniformLocation(shader3D->ID, "texture_diffuse1"), 0);

                if (hasAnimator)
                {
                    auto& animator = entity->getComponent<Animator>();

                    for (int i = 0; i < animator.finalBoneMatrices.size(); i++)
                    {
                        std::string name = "finalBonesMatrices[" + std::to_string(i) + "]";
                        glUniformMatrix4fv(
                            glGetUniformLocation(shader3D->ID, name.c_str()),
                            1,
                            GL_FALSE,
                            glm::value_ptr(animator.finalBoneMatrices[i])
                        );
                    }
                }
                else
                {
                    // No animation → identity matrices
                    for (int i = 0; i < 100; i++)
                    {
                        std::string name = "finalBonesMatrices[" + std::to_string(i) + "]";
                        glUniformMatrix4fv(
                            glGetUniformLocation(shader3D->ID, name.c_str()),
                            1,
                            GL_FALSE,
                            glm::value_ptr(glm::mat4(1.0f))
                        );
                    }
                }
                // render each mesh
                for (auto& mesh : *model.meshes) {
                    mesh.Draw(*shader3D);
                }
            }
        }
    }
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_RENDERSYSTEM_H