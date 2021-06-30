//
// Created by JS on 30/05/2021.
//

#include "engine/prefab/SimplePointLight.hpp"

namespace en
{
    SimplePointLight::SimplePointLight(glm::vec3 color, float strength, const std::string& name) :
            Model("sphere.obj", true, name),
            PointLight(strength)
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
