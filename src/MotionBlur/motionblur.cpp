//
// Created by Nika on 11/06/2021.
//uses create_texture_rgba32f and setup_fullscreen_quad as well as a modified version of build_framebuffer from aufgabe7
//general motionblur technique and motionblur frag shaders based on https://ogldev.org/www/tutorial41/tutorial41.html, https://john-chapman-graphics.blogspot.com/2013/01/per-object-motion-blur.html
#include "MotionBlur/motionblur.hpp"
#include "Engine/Util.hpp"

namespace en{
    unsigned int create_texture_rgba32f(int width, int height) {
        unsigned int handle;
        glCreateTextures(GL_TEXTURE_2D, 1, &handle);
        glTextureStorage2D(handle, 1, GL_RGBA32F, width, height);
        return handle;
    }
    unsigned int setup_fullscreen_quad() {
        float vertices[] = {
                -1.0f, -1.0f, 0.0f,
                1.0f, -1.0f, 0.0f,
                1.0f, 1.0f, 0.0f,
                -1.0f, 1.0f, 0.0f
        };

        unsigned int indices[] = {
                0, 1, 2, 2, 3, 0
        };

        unsigned int VAO;
        glGenVertexArrays(1, &VAO);
        glBindVertexArray(VAO);

        unsigned int VBO = makeBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(vertices), vertices);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
        glEnableVertexAttribArray(0);

        unsigned int IBO = makeBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, sizeof(indices), indices);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

        return VAO;
    }

    void motionblur::addskeletalarrays(GLProgram program) {
        assert(prevtransforms.size()==currenttransforms.size());
        for (int i = 0; i < currenttransforms.size(); ++i) {
            program.SetUniformMat4(("finalbones[" + std::to_string(i) + "]").c_str(), false, glm::value_ptr(currenttransforms[i]));
            //en::Log::Info(glm::to_string(transforms[i]));
            program.SetUniformMat4(("prevfinalbones[" + std::to_string(i) + "]").c_str(), false, glm::value_ptr(prevtransforms[i]));
            //en::Log::Info(glm::to_string(transforms[i]));
        };
    }
    void motionblur::build_framebuffer(int width, int height) {
        if (color_tex) {
            glDeleteTextures(1, &color_tex);
        }

        if (depth_rbo) {
            glDeleteRenderbuffers(1, &depth_rbo);
        }

        if (fbo) {
            glDeleteFramebuffers(1, &fbo);
        }
        if (motion_tex){
            glDeleteTextures(1, &motion_tex);
        }

        color_tex = en::create_texture_rgba32f(width, height);
        motion_tex = en::create_texture_rgba32f(width, height);


        glCreateRenderbuffers(1, &depth_rbo);
        glNamedRenderbufferStorage(depth_rbo, GL_DEPTH_COMPONENT32, width, height);
        glCreateFramebuffers(1, &fbo);
        glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT0, color_tex, 0);
        glNamedFramebufferTexture(fbo, GL_COLOR_ATTACHMENT1, motion_tex, 0);
        //glFramebufferTexture2D(ssaofbo, GL_COLOR_ATTACHMENT0,GL_TEXTURE_2D, color_tex, 0);
        glNamedFramebufferRenderbuffer(fbo, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depth_rbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        GLenum DrawBuffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
        glNamedFramebufferDrawBuffers(fbo, 2, DrawBuffers);
        if (glCheckNamedFramebufferStatus(fbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            printf("Incomplete FBO!");
            std::terminate();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //Log::Info("Built framebuffer"+std::to_string(color_tex)+std::to_string(motion_tex));
    }
    motionblur::motionblur(int width, int height) {
        build_framebuffer(width, height);
        quad = en::setup_fullscreen_quad();
        //Log::Info("Build done"+std::to_string(quad));
    }
    void motionblur::updatetransforms(std::vector<glm::mat4> newtransforms) {
        prevtransforms = currenttransforms;
        currenttransforms = newtransforms;
    }
    GLProgram motionblur::makerenderprog() {
        en::GLShader vertShader("motionblur.vert", en::GLShader::Type::VERTEX);
        en::GLShader fragShader("motionblur.frag", en::GLShader::Type::FRAGMENT);
        en::GLProgram program(&vertShader, nullptr, &fragShader);
        //Log::Info("Got renderprog");
        return program;
    }
    void motionblur::doblur(GLProgram* program) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        program->Use();
        glDisable(GL_DEPTH_TEST);
        glBindVertexArray(quad);
        glBindTextureUnit(0, color_tex);
        glBindTextureUnit(1, motion_tex);
        program->SetUniformI("colortex", 0);
        program->SetUniformI("motiontex", 1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*) 0);
        //Log::Info("shouldve drawn this");
    }
    void motionblur::firstpasssetup(int width, int height) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0,0,width, height);
    }
    void motionblur::addprevprojviewmodelmat(GLProgram program) {
        program.SetUniformMat4("prevPVM", false, glm::value_ptr(prevprojviewmodelmat));
    }
}