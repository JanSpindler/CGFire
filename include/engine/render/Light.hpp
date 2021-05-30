//
// Created by JS on 26/05/2021.
//

#ifndef CGFIRE_LIGHT_H
#define CGFIRE_LIGHT_H

#include <glm/glm.hpp>
#include "GLShader.hpp"
#include <vector>

namespace en
{
    class DirLight
    {
    public:
        DirLight(glm::vec3 dir, glm::vec3 color);

        void Use(const GLProgram* program) const;

        void SetDir(glm::vec3 dir);
        void SetColor(glm::vec3 color);

    private:
        glm::vec3 dir_;
        glm::vec3 color_;
    };

    // Concrete PointLight must provide own pos and color
    class PointLight
    {
    public:
        PointLight();

        virtual glm::vec3 GetPos() const = 0;
        virtual glm::vec3 GetColor() const = 0;
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
