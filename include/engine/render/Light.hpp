//
// Created by JS on 26/05/2021.
//

#ifndef CGFIRE_LIGHT_H
#define CGFIRE_LIGHT_H

#include <glm/glm.hpp>
#include "GLShader.hpp"
#include <vector>
#include "GLTexture.hpp"

#define POINT_LIGHT_MAX 338

namespace en
{
    class DirLight
    {
    public:
        DirLight(glm::vec3 dir, glm::vec3 color);

        // Light
        void Use(const GLProgram* program) const;

        void SetDir(glm::vec3 dir);
        void SetColor(glm::vec3 color);

        // Shadow
        void UseShadow(const GLProgram* program) const;
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

        void Use(const GLProgram* program, unsigned int index) const;

        // Light
        virtual glm::vec3 GetPos() const = 0;
        virtual glm::vec3 GetColor() const = 0;
        float GetStrength() const;

        // Shadow
        void UseShadow(const GLProgram* program) const;
        glm::mat4 GetLightMat() const;
        void BindShadowBuffer() const;
        void UnbindShadowBuffer() const;

    private:
        float strength_;
        unsigned int shadowFbo_;
        GLDepthCubeMap depthCubeMap_;
    };
}

#endif //CGFIRE_LIGHT_H
