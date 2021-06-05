//
// Created by Nika on 29/05/2021.
//AssimptoGLM4x4 based on https://stackoverflow.com/questions/29184311/how-to-rotate-a-skinned-models-bones-in-c-using-assimp
#pragma once
#ifndef CGFIRE_ASSIMPTOGLMMATRIX_H
#define CGFIRE_ASSIMPTOGLMMATRIX_H
#include<assimp/matrix4x4.h>
#include<glm/glm.hpp>
#include <glm/detail/type_quat.hpp>

namespace util{
    inline glm::mat4 AssimptoGLM4x4(aiMatrix4x4 assimpmat){
        glm::mat4 glmmat;
        glmmat[0][0] = assimpmat.a1;glmmat[1][0]=assimpmat.a2;glmmat[2][0]=assimpmat.a3;glmmat[3][0]=assimpmat.a4;
        glmmat[0][1] = assimpmat.b1;glmmat[1][1]=assimpmat.b2;glmmat[2][1]=assimpmat.b3;glmmat[3][1]=assimpmat.b4;
        glmmat[0][2] = assimpmat.c1;glmmat[1][2]=assimpmat.c2;glmmat[2][2]=assimpmat.c3;glmmat[3][2]=assimpmat.c4;
        glmmat[0][3] = assimpmat.d1;glmmat[1][3]=assimpmat.d2;glmmat[2][3]=assimpmat.d3;glmmat[3][3]=assimpmat.d4;
        return glmmat;
    }
    inline glm::vec3 AssimptoGLMvec3(aiVector3D assimpvec){
        glm::vec3 glmvec = glm::vec3(assimpvec.x, assimpvec.y, assimpvec.z);
        return glmvec;
    }
    inline glm::quat AssimptoGLmquat(aiQuaternion assimpquat){
        glm::quat glmquat = glm::quat(assimpquat.w, assimpquat.x, assimpquat.y, assimpquat.z);
        return glmquat;
    }

}

#endif //CGFIRE_ASSIMPTOGLMMATRIX_H
