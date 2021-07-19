//
// Created by JS on 07/07/2021.
//

#ifndef CGFIRE_BACKGROUNDTERRAIN_HPP
#define CGFIRE_BACKGROUNDTERRAIN_HPP

#include <vector>
#include <glm/glm.hpp>
#include "engine/render/Renderable.hpp"

namespace en
{
    class BackgroundTerrain : public RenderObj
    {
    public:
        BackgroundTerrain(
                uint32_t vertexCount,
                float vertexSpacing,
                float baseHeight,
                float baseFreq,
                float amplitude,
                float seed);
        ~BackgroundTerrain();

        void RenderPosOnly(const GLProgram* program) override;
        void RenderDiffuse(const GLProgram* program) override;
        void RenderAll(const GLProgram* program) override;

        bool IsRenderObjTypePossible(RenderObjType type) const override;
    private:
        float seed_;

        uint32_t vao_;
        uint32_t indexCount_;

        std::vector<std::vector<glm::vec3>> GenHeightMap(
                uint32_t vertexCount,
                float vertexSpacing,
                float baseHeight,
                float baseFreq,
                float amplitude);
        float RandomHeight(glm::vec2 pos, float baseFreq, float seed);

        void GenVao(const std::vector<std::vector<glm::vec3>>& heightMap);
    };
}

#endif //CGFIRE_BACKGROUNDTERRAIN_HPP
