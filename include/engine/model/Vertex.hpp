//
// Created by JS on 26/05/2021.
//
//Annika added skeletal animation support 29/05/21, based on https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#ifndef CGFIRE_VERTEX_HPP
#define CGFIRE_VERTEX_HPP

#define MAXBONEINFLUENCE 4

#include <glm/glm.hpp>

namespace en
{
    struct Vertex
    {
        glm::vec3 pos_{};
        glm::vec3 normal_{};
        glm::vec2 uv_{};
        int boneids_[MAXBONEINFLUENCE]{};
        float boneweights_[MAXBONEINFLUENCE]{};

        Vertex(glm::vec3 pos);
        Vertex(glm::vec3 pos, glm::vec3 normal);
        Vertex(glm::vec3 pos, glm::vec2 uv);
        Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv);
        Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv, int boneids[MAXBONEINFLUENCE], float boneweights[MAXBONEINFLUENCE]);
    };
}

#endif //CGFIRE_VERTEX_HPP
