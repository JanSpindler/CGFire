//
// Created by JS on 31/05/2021.
//

#ifndef CGFIRE_GLDEPTHMAP_HPP
#define CGFIRE_GLDEPTHMAP_HPP

#include "GLTexture.hpp"

namespace en
{
    class GLDepthTex : public GLTexture
    {
    public:
        GLDepthTex(int width, int height);

        void Bind() const override;
        void BindToFramebuffer() const;

    private:
    };

    class GLDepthMap
    {
    public:
        GLDepthMap();

        void Bind() const;
        void UnbindFramebuffer() const;

    private:
        static const unsigned int WIDTH_ = 1024;
        static const unsigned int HEIGHT_ = 1024;

        unsigned int fbo_;
        GLDepthTex tex_;
    };
}

#endif //CGFIRE_GLDEPTHMAP_HPP
