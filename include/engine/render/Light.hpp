//
// Created by JS on 26/05/2021.
//

#ifndef CGFIRE_LIGHT_H
#define CGFIRE_LIGHT_H

#include <glm/glm.hpp>
#include "GLShader.hpp"
#include <vector>
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

    class DirLight
    {
    public:
        DirLight(glm::vec3 dir, glm::vec3 color);

        // Light
        void Use(const GLProgram* program) const;

        void SetDir(glm::vec3 dir);
        void SetColor(glm::vec3 color);

        // Shadow
        void UseShadow(const GLProgram* program);
        glm::mat4 GetLightMat() const;
        void BindShadowBuffer() const;
        void UnbindShadowBuffer() const;

    private:
        glm::vec3 dir_;
        glm::vec3 color_;
        unsigned int shadowFbo_;
        GLDepthTex depthTex_;
    };

    // Concrete PointLight must provide own pos and color
    class PointLight
    {
    public:
        PointLight(float strength);

        virtual glm::vec3 GetPos() const = 0;
        virtual glm::vec3 GetColor() const = 0;
        float GetStrength() const;

    private:
        float strength_;
    };

    class PointLightBatch
    {
    public:
        PointLightBatch(const std::vector<const PointLight*>& pointLights);

        void Use(const GLProgram* program);

        void AddPointLight(const PointLight* pointLight);
        void RemovePointLight(const PointLight* pointLight);

    private:
        std::vector<const PointLight*> pointLights_;
    };
}

#endif //CGFIRE_LIGHT_H
