//
// Created by adamd on 2026-01-14.
//

#include "Map.h"

#include <cmath>
#include <iostream>

#include "TextureManager.h"
#include <sstream>
#include <tinyxml2.h>

void Map::load(const char *path, SDL_Texture *ts) {

    tileset = ts;
    tinyxml2::XMLDocument doc;
    doc.LoadFile(path);

    if (doc.Error()) {
        std::cout << "Map doc did not load with path: " << path << std::endl;
    }

    // parse width and height of map
    auto* mapNote = doc.FirstChildElement("map");
    if (!mapNote) {
        std::cout << "No <map> element found!" << std::endl;
        return;
    }
    width = mapNote->IntAttribute("width");
    height = mapNote->IntAttribute("height");
    tileWidth = mapNote->IntAttribute("tilewidth");
    tileHeight = mapNote->IntAttribute("tileheight");

    // parse terrain data
    auto* layer = mapNote->FirstChildElement("layer");
    if (!layer) {
        std::cout << "No <layer> element found!" << std::endl;
        return;
    }
    auto* data = layer->FirstChildElement("data");

    std::string csv = data->GetText();
    std::stringstream ss(csv);

    tileData = std::vector(height, std::vector<int>(width));
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            std::string val;
            // read characters from a ss into val until it hits a comma or is at the end of the stream
            if (!std::getline(ss, val, ',')) break;
            tileData[i][j] = std::stoi(val); // stoi is a string integer converter
        }
    }

    // code for loop of all siblings
    for (auto* objectGroup = layer->NextSiblingElement("objectgroup");
        objectGroup != nullptr;
        objectGroup = objectGroup->NextSiblingElement("objectgroup")) {

        const std::string name = objectGroup->Attribute("name");
        if (name == "Collision Layer") {
            // create a for loop with initialization and an increment
            for (auto* obj = objectGroup->FirstChildElement("object");
                obj != nullptr;
                obj = obj->NextSiblingElement("object")) {

                Collider c;
                c.rect.x = obj->FloatAttribute("x");
                c.rect.y = obj->FloatAttribute("y");
                c.rect.w = obj->FloatAttribute("width");
                c.rect.h = obj->FloatAttribute("height");
                colliders.push_back(c);
                }
        }

        if (name == "Item Layer") {
            for (auto* obj = objectGroup->FirstChildElement("object");
                obj != nullptr;
                obj = obj->NextSiblingElement("object")) {

                Collider c;
                c.rect.x = obj->FloatAttribute("x");
                c.rect.y = obj->FloatAttribute("y");
                c.rect.w = 32;
                c.rect.h = 32;
                items.push_back(c);
                }
        }

        if (name == "Spawner Layer") {
            for (auto* obj = objectGroup->FirstChildElement("object");
                obj != nullptr;
                obj = obj->NextSiblingElement("object")) {

                Collider c;
                c.rect.x = obj->FloatAttribute("x");
                c.rect.y = obj->FloatAttribute("y");
                c.rect.w = 32;
                c.rect.h = 32;
                spawner.push_back(c);
                }
        }
    }
}

void Map::draw(const Camera &cam) {

    SDL_FRect src {}, dest {};

    dest.w = tileWidth;
    dest.h = tileHeight;

    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            int type = tileData[row][col];

            float worldX = static_cast<float>(col) * dest.w;
            float worldY = static_cast<float>(row) * dest.h;

            // move the tiles pr map relative to the camera
            // convert from world space to screen space
            dest.x = std::round(worldX - cam.view.x);
            dest.y = std::round(worldY - cam.view.y);

            switch (type) {
                case 1:
                    // dirt
                    src.x = 0;
                    src.y = 0;
                    src.w = tileWidth;
                    src.h = tileHeight;
                    break;
                case 2:
                    // grass
                    src.x = tileWidth;
                    src.y = 0;
                    src.w = tileWidth;
                    src.h = tileHeight;
                    break;
                case 4:
                    // water
                    src.x = tileWidth;
                    src.y = tileHeight;
                    src.w = tileWidth;
                    src.h = tileHeight;
                    break;
                default:
                    break;
            }
            TextureManager::draw(tileset, src, dest);
        }
    }
}