//
// Created by Nika on 11/06/2021.
//

#ifndef CGFIRE_MOTIONBLUR_HPP
#define CGFIRE_MOTIONBLUR_HPP
#include "engine/render/GLShader.hpp"
#include "engine/render/Renderable.hpp"
#include "framework/common.hpp"
#include "framework/buffer.hpp"

namespace en{
    class motionblur
    {
    public:
        std::vector<glm::mat4> currenttransforms;
        std::vector<glm::mat4> prevtransforms;
        glm::mat4 prevprojviewmodelmat = glm::mat4();
        void doblur(GLProgram* program);
        motionblur(int width, int height);
        void updatetransforms(std::vector<glm::mat4> newtransforms);
        GLuint color_tex = 0;
        GLuint depth_rbo = 0;
        GLuint fbo = 0;
        GLuint motion_tex = 0;
        void addskeletalarrays(GLProgram program);
        void addprevprojviewmodelmat(GLProgram program);
        void firstpasssetup(int width, int height);
        GLProgram makerenderprog();
        void build_framebuffer(int width, int height);
        unsigned int quad;
    };
}

#endif //CGFIRE_MOTIONBLUR_HPP
