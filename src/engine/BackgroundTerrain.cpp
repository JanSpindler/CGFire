//
// Created by JS on 07/07/2021.
//

#include "engine/prefab/BackgroundTerrain.hpp"
#include "engine/gr_include.hpp"
#include "engine/Util.hpp"
#include <glm/gtc/random.hpp>
#include <glm/gtc/noise.hpp>

namespace en
{
    BackgroundTerrain::BackgroundTerrain(
            uint32_t vertexCount,
            float vertexSpacing,
            float baseHeight,
            float baseFreq,
            float amplitude,
            float seed) :
        RenderObj(),
        seed_(seed)
    {
        GenVao(GenHeightMap(vertexCount, vertexSpacing, baseHeight, baseFreq, amplitude));
    }

    BackgroundTerrain::~BackgroundTerrain()
    {
    }

    void BackgroundTerrain::RenderPosOnly(const GLProgram* program) const
    {
        RenderObj::SetMatrices(program);
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void BackgroundTerrain::RenderDiffuse(const GLProgram* program) const
    {
        RenderObj::SetMatrices(program);
        program->SetUniformB("use_diffuse_tex", false);
        program->SetUniformVec4f("diffuse_color", glm::vec4(1.0f));
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void BackgroundTerrain::RenderAll(const GLProgram* program) const
    {
        RenderObj::SetMatrices(program);
        program->SetUniformB("use_diffuse_tex", false);
        program->SetUniformVec4f("diffuse_color", glm::vec4(1.0f));
        program->SetUniformB("use_specular_tex", false);
        program->SetUniformVec4f("specular_color", glm::vec4(0.0f));
        program->SetUniformF("shininess", 0.0f);
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    std::vector<std::vector<glm::vec3>> BackgroundTerrain::GenHeightMap(
            uint32_t vertexCount,
            float vertexSpacing,
            float baseHeight,
            float baseFreq,
            float amplitude)
    {
        float totalSize = vertexSpacing * (float) vertexCount;
        float baseCoord = -0.5f * totalSize;
        glm::vec3 basePoint(baseCoord, baseHeight, baseCoord);

        std::vector<std::vector<glm::vec3>> heightMap(vertexCount);
        for (uint32_t x = 0; x < vertexCount; x++)
        {
            heightMap[x].resize(vertexCount);
            for (uint32_t z = 0; z < vertexCount; z++)
            {
                float xOffset = vertexSpacing * (float)x;
                float zOffset = vertexSpacing * (float)z;

                float distance = glm::length(glm::vec2(xOffset + baseCoord, zOffset + baseCoord));
                float d = glm::clamp((distance - 128.0f) / 256.0f, 0.0f, 1.0f);

                float yOffset = d * amplitude * RandomHeight(glm::vec2(xOffset, zOffset) / totalSize, baseFreq, seed_);

                heightMap[x][z] = basePoint + glm::vec3(xOffset, yOffset, zOffset);
            }
        }

        return heightMap;
    }

    //https://www.redblobgames.com/maps/terrain-from-noise/
    float BackgroundTerrain::RandomHeight(glm::vec2 pos, float baseFreq, float seed)
    {
        const float persistance = 0.5f;
        const float lacunarity = 2.0f;
        const uint32_t octaveCount = 16;
        const float exponent = 1.0f;

        float freq = baseFreq;
        float ampl = 1.0f;
        float height = 0.0f;
        float norm = 0.0f;
        for (uint32_t i = 0; i < octaveCount; i++)
        {
            float perlin = glm::perlin(pos * freq + glm::vec2(seed)) + 0.5f;
            height += perlin * ampl;
            norm += ampl;
            ampl *= persistance;
            freq *= lacunarity;
        }
        height /= norm;
        height = glm::pow(height, exponent);
        height -= 0.5f;
        //Log::Info(std::to_string(height));
        return height;
    }

    struct TVertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;

        TVertex(glm::vec3 nPos, glm::vec3 nNormal, glm::vec2 nUv) :
            pos(nPos),
            normal(nNormal),
            uv(nUv)
        {
        }
    };

    void BackgroundTerrain::GenVao(const std::vector<std::vector<glm::vec3>>& heightMap)
    {
        uint32_t vertexCount = heightMap.size();

        std::vector<TVertex> vertices;
        for (uint32_t x = 0; x < vertexCount; x++)
        {
            for (uint32_t z = 0; z < vertexCount; z++)
            {
                glm::vec3 normal(0.0f, 1.0f, 0.0f);
                if (x < vertexCount - 1 && z < vertexCount - 1)
                    normal = -glm::normalize(glm::cross(heightMap[x + 1][z] - heightMap[x][z], heightMap[x][z + 1] - heightMap[x][z]));
                vertices.emplace_back(heightMap[x][z], normal, glm::vec2(0.0f));
            }
        }

        std::vector<uint32_t> indices;
        for (uint32_t x = 0; x < vertexCount - 1; x++)
        {
            for (uint32_t z = 0; z < vertexCount - 1; z++)
            {
                uint32_t i0 = x * vertexCount + z;
                uint32_t i1 = (x + 1) * vertexCount + z;
                uint32_t i2 = (x + 1) * vertexCount + (z + 1);
                uint32_t i3 = x * vertexCount + (z + 1);
                std::vector<uint32_t> quadIndices = { i0, i3, i2, i2, i1, i0 };
                indices.insert(indices.end(), quadIndices.begin(), quadIndices.end());
            }
        }

        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        uint32_t vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(TVertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TVertex), (void*) offsetof(TVertex, TVertex::pos));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TVertex), (void*) offsetof(TVertex, TVertex::normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(TVertex), (void*) offsetof(TVertex, TVertex::uv));

        uint32_t ibo;
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);

        indexCount_ = indices.size();
    }
}
