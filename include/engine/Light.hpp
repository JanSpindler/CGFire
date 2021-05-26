//
// Created by JS on 26/05/2021.
//

#ifndef CGFIRE_LIGHT_H
#define CGFIRE_LIGHT_H

#include <glm/glm.hpp>

namespace en
{
    struct DirectionalLight
    {
        glm::vec3 dir_;
        glm::vec4 color_;
    };

    struct PointLight
    {
        glm::vec3 pos_;
        glm::vec4 color_;

        bool operator==(const PointLight& other) const;
    };
}

#endif //CGFIRE_LIGHT_H
