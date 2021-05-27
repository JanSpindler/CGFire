//
// Created by JS on 26/05/2021.
//

#ifndef CGFIRE_LIGHT_H
#define CGFIRE_LIGHT_H

#include <glm/glm.hpp>
#include "GLShader.hpp"

namespace en
{
    struct DirLight
    {
        glm::vec3 dir_;
        glm::vec3 color_;

        DirLight(glm::vec3 dir, glm::vec3 color);

        void Use(const GLProgram* program) const;
    };

    struct PointLight
    {
        glm::vec3 pos_;
        glm::vec3 color_;

        PointLight(glm::vec3 pos, glm::vec3 color);

        void Use(const GLProgram* program, unsigned int index) const;

        bool operator==(const PointLight& other) const;
    };
}

#endif //CGFIRE_LIGHT_H
