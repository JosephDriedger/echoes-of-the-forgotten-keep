//
// Created by adamd on 2026-01-07.
//

#include "Game.h"
#include "Map.h"
#include <vendor/glm/glm.hpp>
#include <vendor/glm/gtc/matrix_transform.hpp>
#include <vendor/glm/gtc/type_ptr.hpp>

#include <iostream>
#include <ostream>

#include "manager/AssetManager.h"
#include "Collision.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//#include "GameObject.h"

//GameObject *player = nullptr;

int updateColor = 1000;

std::function<void(std::string)> Game::onSceneChangeRequest;

Game::Game() {}

Game::~Game() {
    destroy();
}

void Game::init(const char *title, int width, int height, bool fullscreen) {

    int flags = 0;
    if (fullscreen) {

        flags = SDL_WINDOW_FULLSCREEN;
    }

    // initialize SDL library
    if (SDL_InitSubSystem(SDL_INIT_VIDEO) == 1) {
        std::cout << "Subsystem initialized..." << std::endl;

        // Tell SDL we want OpenGL 3.3 Core
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);


        window = SDL_CreateWindow(title, width, height, SDL_WINDOW_OPENGL);
        if (window) {
            std::cout << "Window created..." << std::endl;
        }

        // windows will be direct3D (directx)
        // Mac is will likely be metal, OpenGL
        renderer = SDL_CreateRenderer(window, nullptr);

        if (renderer) {
            std::cout << "renderer created..." << std::endl;
        }
        else {
            std::cout << "renderer could not be created..." << std::endl;
        }

        // Create OpenGL context
        glContext = SDL_GL_CreateContext(window);

        if (glContext) {
            std::cout << "OpenGL context created..." << std::endl;
        } else {
            std::cout << "Failed to create OpenGL context..." << std::endl;
        }

        SDL_GL_MakeCurrent(window, glContext);
        if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
            std::cout << "Failed to initialize GLAD!" << std::endl;
        }

        shader = new Shader(
            "../src/renderer/shaders/vertex.glsl",
            "../src/renderer/shaders/fragment.glsl");
        week4output();

        isRunning = true;
    }
    else {
        isRunning = false;
    }

    // load assets
    AssetManager::loadAnimation("player", "../asset/animations/Swordsman_animations.xml");
    AssetManager::loadAnimation("enemy", "../asset/animations/bird_animations.xml");

    //load scenes
    sceneManager.loadScene("level1", "../asset/map.tmx", width, height);
    sceneManager.loadScene("level2", "../asset/map2.tmx", width, height);

    // start level 1
    sceneManager.changeSceneDeferred("level1");

    // resolve scene callback
    onSceneChangeRequest = [this](std::string sceneName) {

        if (sceneManager.currentScene->getName() == "level2" && sceneName == "level2") {
            std::cout << "You win!" << std::endl;
            isRunning = false;
            return;
        }

        if (sceneName == "gameover") {
            std::cout << "Game Over!" << std::endl;

            isRunning = false;
            return;
        }

        sceneManager.changeSceneDeferred(sceneName);
    };

   //  float vertices[] = {
   //      0.0f,  0.5f, 0.0f,
   //     -0.5f, -0.5f, 0.0f,
   //      0.5f, -0.5f, 0.0f
   // };


    // // 1. Create an Assimp importer
    // Assimp::Importer importer;
    //
    // // 2. Load your GLB or GLTF file
    // const aiScene* scene = importer.ReadFile(
    //     "../asset/Knight.glb",  // path relative to your executable
    //     aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
    // );
    //
    // // 3. Check if the model loaded
    // if (!scene) {
    //     std::cerr << "Error loading model: " << importer.GetErrorString() << std::endl;
    //
    // }
    //
    // std::cout << "Model loaded successfully!\n";
    //
    // // 4. Print mesh names (for testing)
    // for (unsigned int i = 0; i < scene->mNumMeshes; i++) {
    //     std::cout << "Mesh " << i << ": " << scene->mMeshes[i]->mName.C_Str() << std::endl;
    // }
    //
    // for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    // {
    //     aiMesh* mesh = scene->mMeshes[i];
    //
    //     std::vector<Vertex> vertices;
    //     std::vector<unsigned int> indices;
    //
    //     for (unsigned int v = 0; v < mesh->mNumVertices; v++)
    //     {
    //         Vertex vertex;
    //
    //         vertex.Position = glm::vec3(
    //             mesh->mVertices[v].x,
    //             mesh->mVertices[v].y,
    //             mesh->mVertices[v].z);
    //
    //         if (mesh->HasNormals())
    //         {
    //             vertex.Normal = glm::vec3(
    //                 mesh->mNormals[v].x,
    //                 mesh->mNormals[v].y,
    //                 mesh->mNormals[v].z);
    //         }
    //
    //         if (mesh->mTextureCoords[0])
    //         {
    //             vertex.TexCoords = glm::vec2(
    //                 mesh->mTextureCoords[0][v].x,
    //                 mesh->mTextureCoords[0][v].y);
    //         }
    //
    //         vertices.push_back(vertex);
    //     }
    //
    //     for (unsigned int f = 0; f < mesh->mNumFaces; f++)
    //     {
    //         aiFace face = mesh->mFaces[f];
    //
    //         for (unsigned int j = 0; j < face.mNumIndices; j++)
    //             indices.push_back(face.mIndices[j]);
    //     }
    //
    //     Mesh newMesh(vertices, indices);
    //     meshes.push_back(newMesh);
    // }


    glEnable(GL_DEPTH_TEST);

    // Cube vertex data (positions + texture coords)
    // float cubeVertices[] = {
    //     // positions          // tex coords
    //     -0.5f,-0.5f,-0.5f, 0.0f,0.0f,
    //      0.5f,-0.5f,-0.5f, 1.0f,0.0f,
    //      0.5f, 0.5f,-0.5f, 1.0f,1.0f,
    //      0.5f, 0.5f,-0.5f, 1.0f,1.0f,
    //     -0.5f, 0.5f,-0.5f, 0.0f,1.0f,
    //     -0.5f,-0.5f,-0.5f, 0.0f,0.0f,
    //
    //     -0.5f,-0.5f, 0.5f, 0.0f,0.0f,
    //      0.5f,-0.5f, 0.5f, 1.0f,0.0f,
    //      0.5f, 0.5f, 0.5f, 1.0f,1.0f,
    //      0.5f, 0.5f, 0.5f, 1.0f,1.0f,
    //     -0.5f, 0.5f, 0.5f, 0.0f,1.0f,
    //     -0.5f,-0.5f, 0.5f, 0.0f,0.0f,
    //
    //     -0.5f, 0.5f, 0.5f, 1.0f,0.0f,
    //     -0.5f, 0.5f,-0.5f, 1.0f,1.0f,
    //     -0.5f,-0.5f,-0.5f, 0.0f,1.0f,
    //     -0.5f,-0.5f,-0.5f, 0.0f,1.0f,
    //     -0.5f,-0.5f, 0.5f, 0.0f,0.0f,
    //     -0.5f, 0.5f, 0.5f, 1.0f,0.0f,
    //
    //      0.5f, 0.5f, 0.5f, 1.0f,0.0f,
    //      0.5f, 0.5f,-0.5f, 1.0f,1.0f,
    //      0.5f,-0.5f,-0.5f, 0.0f,1.0f,
    //      0.5f,-0.5f,-0.5f, 0.0f,1.0f,
    //      0.5f,-0.5f, 0.5f, 0.0f,0.0f,
    //      0.5f, 0.5f, 0.5f, 1.0f,0.0f,
    //
    //     -0.5f,-0.5f,-0.5f, 0.0f,1.0f,
    //      0.5f,-0.5f,-0.5f, 1.0f,1.0f,
    //      0.5f,-0.5f, 0.5f, 1.0f,0.0f,
    //      0.5f,-0.5f, 0.5f, 1.0f,0.0f,
    //     -0.5f,-0.5f, 0.5f, 0.0f,0.0f,
    //     -0.5f,-0.5f,-0.5f, 0.0f,1.0f,
    //
    //     -0.5f, 0.5f,-0.5f, 0.0f,1.0f,
    //      0.5f, 0.5f,-0.5f, 1.0f,1.0f,
    //      0.5f, 0.5f, 0.5f, 1.0f,0.0f,
    //      0.5f, 0.5f, 0.5f, 1.0f,0.0f,
    //     -0.5f, 0.5f, 0.5f, 0.0f,0.0f,
    //     -0.5f, 0.5f,-0.5f, 0.0f,1.0f
    // };

    // glGenVertexArrays(1, &cubeVAO);
    // glGenBuffers(1, &cubeVBO);
    //
    // glBindVertexArray(cubeVAO);
    // glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    // positions
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)0);
    // glEnableVertexAttribArray(0);

    // texture coords
    // glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    // glEnableVertexAttribArray(2);
    //
    // glBindVertexArray(0);

    // Load texture (BMP example, or use SDL2_image for PNG)
    // SDL_Surface* surface = IMG_Load("../asset/knight_texture.png");
    // if(!surface) { std::cout << "Failed to load texture!\n"; return; }
    // // glGenTextures(1, &cubeTexture);
    // // glBindTexture(GL_TEXTURE_2D, cubeTexture);
    //
    //
    //
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0,
    //              GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    // glGenerateMipmap(GL_TEXTURE_2D);
    // SDL_DestroySurface(surface);

    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Game::week4output() {

    Vector2D a(3, 4);
    Vector2D b(1, 2);
    std::cout << "a.x " << a.x << " a.y " << a.y << std::endl;
    std::cout << "b.x " << b.x << " b.y " << b.y << std::endl;
    a += b;              // a is now (4, 6)
    std::cout << "a.x " << a.x << " a.y " << a.y << std::endl;
    Vector2D c = a - b;  // c is (3, 4)
    std::cout << "c.x " << c.x << " c.y " << c.y << std::endl;
    Vector2D d = a * 2;  // d is (8, 12)
    std::cout << "d.x " << d.x << " d.y " << d.y << std::endl;
    Vector2D e = 2 * a;  // e is also (8, 12)
    std::cout << "e.x " << e.x << " e.y " << e.y << std::endl;
    Vector2D f = -a;     // f is (-4, -6)
    std::cout << "f.x " << f.x << " f.y " << f.y << std::endl;
    bool same = (a == b); // false
    std::cout << "same " << same << std::endl;
}

void Game::handleEvents() {
    // SDL listens to the OS for input events internally and it ands them to a queue

    // check for event, if an event is available it will remove from the queue and store in event

    // code to add
    // while (SDL_PollEvent(&event)) {

    // code to remove
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_EVENT_QUIT: // usually triggered when the user closes the window
        isRunning = false;
            break;
        default:
            break;
    }
    // }
}

void Game::update(float dt) {
    sceneManager.update(dt, event);
    //frameCount++;
    //std::cout << "Frame count: " << dt << std::endl;
    //player->update(dt);

    //world.update(dt, event);
}

void Game::week1(Uint64 ticks) {
    if (ticks > updateColor) {
        r+=updateColor/100;
        g+=updateColor/100;
        b+=updateColor/100;
        // not needed unsign int
        // if (r > 255) {
        //     r-=255;
        // }
        // if (g > 255) {
        //     g-=255;
        // }
        // if (b > 255) {
        //     b-=255;
        // }
        a=255;
        updateColor += 1000;
    }
}

// GLuint createShader(const char* vertexSrc, const char* fragmentSrc)
// {
//     GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
//     glShaderSource(vertex, 1, &vertexSrc, nullptr);
//     glCompileShader(vertex);
//
//     GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
//     glShaderSource(fragment, 1, &fragmentSrc, nullptr);
//     glCompileShader(fragment);
//
//     GLuint program = glCreateProgram();
//     glAttachShader(program, vertex);
//     glAttachShader(program, fragment);
//     glLinkProgram(program);
//
//     glDeleteShader(vertex);
//     glDeleteShader(fragment);
//
//     return program;
// }

void Game::render(Uint64 ticks)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //shader->use();

    //float time = SDL_GetTicks() / 1000.0f;

    // glm::mat4 model = glm::mat4(1.0f);
    // model = glm::translate(model, glm::vec3(0.0f, -1.0f, 0.0f)); // move down so feet are at origin
    // model = glm::scale(model, glm::vec3(1.0f)); // optional scaling
    //model = glm::rotate(model, time, glm::vec3(1,0,0));
    //model = glm::rotate(model, time*0.7f, glm::vec3(0,1,0));

    // glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0,0,-3.0f));
    // glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

    // GLuint modelLoc = glGetUniformLocation(shader->ID, "model");
    // GLuint viewLoc  = glGetUniformLocation(shader->ID, "view");
    // GLuint projLoc  = glGetUniformLocation(shader->ID, "projection");

    // glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    // glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    // glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    //
    // for (auto& mesh : meshes)
    // {
    //     mesh.Draw(*shader);
    // }

    // glActiveTexture(GL_TEXTURE0);
    // //glBindTexture(GL_TEXTURE_2D, cubeTexture);
    // glUniform1i(glGetUniformLocation(shader->ID, "texture_diffuse1"), 0);
    //
    // glBindVertexArray(cubeVAO);
    //glDrawArrays(GL_TRIANGLES, 0, 36);
    //SDL_RenderClear(renderer);
    sceneManager.render();

    SDL_GL_SwapWindow(window);

    // SDL_SetRenderDrawColor(renderer, r, g, b, a);
    //
    // // every frame the renderer is cleared with the draw color
    // SDL_RenderClear(renderer);
    //
    // sceneManager.render();
    //
    // // display everything that was just drawn
    // // draws it in memory first to a back buffer
    //
    // // swaps the back buffer to the screen
    //SDL_RenderPresent(renderer);
}

void Game::destroy() {
    TextureManager::clean();
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    // Delete OpenGL context
    if (glContext)
    {
        SDL_GL_DestroyContext(glContext);
        glContext = nullptr;
    }

    if (window)
    {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
    std::cout << "Game Destroyed..." << std::endl;
}
