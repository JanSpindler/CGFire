//
// Created by JS on 26/05/2021.
//

#ifndef CGFIRE_LIGHT_H
#define CGFIRE_LIGHT_H

#include <glm/glm.hpp>
#include "GLShader.hpp"
#include <vector>
#include "GLTexture.hpp"

#define SHADOW_TEX_WIDTH 2048
#define SHADOW_TEX_HEIGHT SHADOW_TEX_WIDTH

namespace en
{
    class DirLight
    {
    public:
        DirLight(glm::vec3 dir, glm::vec3 color, uint32_t width, uint32_t height);

        // Light
        void Use(const GLProgram* program) const;

        void SetDir(glm::vec3 dir);
        void SetColor(glm::vec3 color);

        uint32_t GetWidth() const;
        uint32_t GetHeight() const;

        // Shadow
        void UseShadow(const GLProgram* program) const;
        glm::mat4 GetLightMat() const;
        void BindShadowBuffer() const;
        void UnbindShadowBuffer() const;
        void BindEsmTex() const;

        void PrepareGauss5(const GLProgram* gauss5Program) const;

    private:
        glm::vec3 dir_;
        glm::vec3 color_;

        uint32_t width_;
        uint32_t height_;

        uint32_t shadowFbo_;
        GLDepthTex depthTex_;
        uint32_t esmTex_;
        uint32_t esmTmpTex_;
    };

    // Concrete PointLight must provide own pos and color
    class PointLight
    {
    public:
        PointLight(float strength, uint32_t width, uint32_t height);

        void Use(const GLProgram* program, unsigned int index) const;

        // Light
        virtual glm::vec3 GetPos() const = 0;
        virtual glm::vec3 GetColor() const = 0;
        float GetStrength() const;

        // Shadow
        void UseShadow(const GLProgram* program, unsigned int index) const;
        std::vector<glm::mat4> GetLightMats() const;
        void BindShadowBuffer() const;
        void UnbindShadowBuffer() const;
        //void BindEsmCubeMap() const;

    private:
        float strength_;

        uint32_t width_;
        uint32_t height_;

        unsigned int shadowFbo_;
        GLDepthCubeMap depthCubeMap_;
        //uint32_t esmCubeMap_;
        //uint32_t esmTmpTex_;
    };
}

#endif //CGFIRE_LIGHT_H
