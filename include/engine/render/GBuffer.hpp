//
// Created by JS on 18/06/2021.
//

#ifndef CGFIRE_GBUFFER_HPP
#define CGFIRE_GBUFFER_HPP

#include <cstdint>
#include "GLTexture.hpp"
#include "GLShader.hpp"

namespace en
{
    class GBuffer
    {
    public:
        GBuffer(int32_t width, int32_t height);
        ~GBuffer();

        void Bind() const;
        void Unbind() const;
        void UseTextures(const GLProgram* program) const;

    private:
        uint32_t fbo_;
        uint32_t posTex_;
        uint32_t normalTex_;
        uint32_t colorTex_;
        uint32_t depthBuffer_;
    };
}

#endif //CGFIRE_GBUFFER_HPP
