//
// Created by JS on 30/05/2021.
//

#include "engine/prefab/SimplePointLight.hpp"

namespace en
{
    SimplePointLight::SimplePointLight(glm::vec3 color, float strength) :
            Model("sphere.obj", true),
            PointLight(strength)
    {
        color_ = color;
    }

    glm::vec3 SimplePointLight::GetPos() const
    {
        return glm::vec3(t_ * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    }

    glm::vec3 SimplePointLight::GetColor() const
    {
        return color_;
    }
}
