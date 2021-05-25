//
// Created by JS on 25/05/2021.
//

#ifndef CGFIRE_ASSETMANAGER_CPP_H
#define CGFIRE_ASSETMANAGER_CPP_H

#include "engine/AssetManager.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader.h>
#include <unordered_map>

#include "engine/config.hpp"
#include "engine/util.hpp"

namespace en
{
    Mesh AssetManager::LoadMesh(const std::string& filename)
    {
        tinyobj::attrib_t vertAttr;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warnStr;
        std::string errorStr;

        std::string filePath = MODEL_ROOT + "/" + filename;
        if (!tinyobj::LoadObj(&vertAttr, &shapes, &materials, &warnStr, &errorStr, filePath.c_str()))
        {
            Log::Warn(warnStr.c_str());
            Log::Error(errorStr.c_str(), true);
        }

        std::unordered_map<Vertex, unsigned int> vertMap;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        for (tinyobj::shape_t shape : shapes)
        {
            for (tinyobj::index_t index : shape.mesh.indices)
            {
                glm::vec3 pos(
                        vertAttr.vertices[index.vertex_index * 3 + 0],
                        vertAttr.vertices[index.vertex_index * 3 + 1],
                        vertAttr.vertices[index.vertex_index * 3 + 2]);
                glm::vec3 normal(
                        vertAttr.vertices[index.normal_index * 3 + 0],
                        vertAttr.vertices[index.normal_index * 3 + 1],
                        vertAttr.vertices[index.normal_index * 3 + 2]);
                /*glm::vec2 uv(
                        vertAttr.vertices[index.texcoord_index * 2 + 0],
                        vertAttr.vertices[index.texcoord_index * 2 + 1]);*/
                Vertex vert;
                vert.pos = pos;
                vert.normal = normal;
                vert.uv = glm::vec2(0.0f);

                if (vertMap.count(vert) == 0)
                {
                    vertMap[vert] = vertMap.size();
                    vertices.push_back(vert);
                }
                indices.push_back(vertMap[vert]);
            }
        }

        return Mesh(vertices, indices);
    }
}

#endif //CGFIRE_ASSETMANAGER_CPP_H
