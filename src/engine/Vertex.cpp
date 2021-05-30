//
// Created by JS on 26/05/2021.
//

#include "engine/Model/Vertex.hpp"

namespace en
{
    Vertex::Vertex(glm::vec3 pos)
    {
        Vertex(pos, glm::vec3(0.0f), glm::vec2(0.0f));
    }

    Vertex::Vertex(glm::vec3 pos, glm::vec3 normal)
    {
        Vertex(pos, normal, glm::vec2(0.0f));
    }

    Vertex::Vertex(glm::vec3 pos, glm::vec2 uv)
    {
        Vertex(pos, glm::vec3(0.0f), uv);
    }

    Vertex::Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv)
    {
        pos_ = pos;
        normal_ = normal;
        uv_ = uv;
    }
}
