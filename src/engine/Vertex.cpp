//
// Created by JS on 26/05/2021.
//
//Annika added skeletal animation support 29/05/21, based on https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#include "engine/Vertex.hpp"

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
        for (int i =0; i<MAXBONEINFLUENCE;i++){
            boneids_[i]=-1;
            boneweights_[i]=0.0f;
        }
    }
    Vertex::Vertex(glm::vec3 pos, glm::vec3 normal, glm::vec2 uv, int boneids[MAXBONEINFLUENCE], float boneweights[MAXBONEINFLUENCE])
    {
        pos_ = pos;
        normal_ = normal;
        uv_ = uv;
        for (int i =0; i<MAXBONEINFLUENCE;i++){
            boneids_[i]=boneids[i];
            boneweights_[i]=boneweights[i];
        }

    }
}
