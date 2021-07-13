//
// Created by JS on 30/05/2021.
//

#include "engine/prefab/SimplePointLight.hpp"

namespace en
{
    SimplePointLight::SimplePointLight(glm::vec3 color, float strength, uint32_t width, uint32_t height, const std::string& name) :
            Model("sphere.obj", true, name),
            PointLight(strength, width, height)
    {
        color_ = color;
    }

    glm::vec3 SimplePointLight::GetPos() const
    {
        return Position;
    }

    glm::vec3 SimplePointLight::GetColor() const
    {
        return color_;
    }
}
