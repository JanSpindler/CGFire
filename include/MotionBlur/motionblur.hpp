//
// Created by Nika on 11/06/2021.
//

#ifndef CGFIRE_MOTIONBLUR_HPP
#define CGFIRE_MOTIONBLUR_HPP
#include "engine/render/GLShader.hpp"
#include "engine/render/Renderable.hpp"
#include "framework/common.hpp"
#include "framework/buffer.hpp"
#include "engine/render/GBuffer.hpp"
#include "engine/Camera.hpp"

namespace en{
    class motionblur
    {
    public:
        std::vector<glm::mat4> currenttransforms;
        std::vector<glm::mat4> prevtransforms;
        glm::mat4 prevprojviewmodelmat = glm::mat4();
        void doblur(const GLProgram* program, const GBuffer* buffer, const Camera* cam) const;
        motionblur(int width, int height);
        void updatetransforms(std::vector<glm::mat4>& newtransforms);
        GLuint color_tex = 0;
        GLuint depth_rbo = 0;
        GLuint fbo = 0;
        GLuint motion_tex = 0;
        void addskeletalarrays(const GLProgram* program);
        void addprevprojviewmodelmat(const GLProgram* program);
        void firstpasssetup(int width, int height) const;
        const GLProgram* makerenderprog();
        void build_framebuffer(int width, int height);
        unsigned int quad;
        void setprevprojviewmodelmat(glm::mat4& viewmat, glm::mat4& projmat);
    };
}

#endif //CGFIRE_MOTIONBLUR_HPP
