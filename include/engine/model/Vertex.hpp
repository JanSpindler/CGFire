//
// Created by JS on 26/05/2021.
//

#ifndef CGFIRE_VERTEX_HPP
#define CGFIRE_VERTEX_HPP

#include <glm/glm.hpp>

namespace en
{
    struct Vertex
    {
        glm::vec3 pos_;
        glm::vec3 normal_;
        glm::vec2 uv_;

        Vertex(glm::vec3 pos);
        Vertex(glm::vec3 pos, glm::vec3 normal);
        Vertex(glm::vec3 pos, glm::vec2 uv);
        Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv);
    };
}

#endif //CGFIRE_VERTEX_HPP
