//
// Created by JS on 26/05/2021.
//

#ifndef CGFIRE_TEXTURE_H
#define CGFIRE_TEXTURE_H

#include <string>

namespace en
{
    class GLTexture
    {
    public:
        GLTexture();
        ~GLTexture();

        virtual void BindTex() const = 0;

    protected:
        unsigned int handle_;
    };

    class GLPictureTex : public GLTexture
    {
    public:
        enum class WrapMode
        {
            REPEAT = 1,
            MIRRORED_REPEAT = 2,
            CLAMP_TO_EDGE = 3,
            CLAMP_TO_BORDER = 4
        };

        enum class FilterMode
        {
            NEAREST = 1,
            LINEAR = 2,
            // TODO: implement MIPMAPs
        };

        GLPictureTex(WrapMode wrapping, FilterMode filterMode, const std::string& path, bool flipUv);
        ~GLPictureTex();

        void BindTex() const override;
    private:
    };

    class GLDepthTex : public GLTexture
    {
    public:
        GLDepthTex(int width, int height);

        void BindTex() const override;
        void BindToFramebuffer() const;

    private:
    };

    class GLDepthCubeMap : public GLTexture
    {
    public:
        GLDepthCubeMap(unsigned int width, unsigned int height);

        void BindTex() const override;
        void BindToFramebuffer() const;

    private:
    };
}

#endif //CGFIRE_TEXTURE_H
