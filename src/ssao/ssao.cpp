//
// Created by Nika on 28/06/2021.
//
//uses setup_fullscreen_quad from aufgabe7
//based on https://learnopengl.com/Advanced-Lighting/SSAO, https://john-chapman-graphics.blogspot.com/2013/01/ssao-tutorial.html
#include <cmath>
#include "ssao/ssao.h"
#include <glad/glad.h>
//#include <GLFW/glfw3.h>
#include "framework/buffer.hpp"
#include "engine/Util.hpp"
#include "engine/Camera.hpp"
#include <glm/gtx/string_cast.hpp>


namespace en{
    void ssao::makekernels() {
        for (int i=0; i<kernelsize; i++){
            glm::vec3 currentkernel (
                    util::Random::Float() *2-1,
                    util::Random::Float()*2-1,
                    util::Random::Float()*2-1
                    );
            currentkernel = glm::normalize(currentkernel);
            currentkernel *= util::Random::Float();
            float scale = (float)i/kernelsize;
            currentkernel *= 0.1f + (0.9f)*(scale*scale);
            ssao::kernel.push_back(currentkernel);
        }
    }
    void ssao::makenoise() {
        for (int i=0; i<16; i++){
            glm::vec3 newnoise (
                    util::Random::Float()*2-1,
                    util::Random::Float()*2-1,
                    0.0f
                    );
            newnoise = glm::normalize(newnoise);
            ssao::noise.push_back(newnoise);
        }
    }
    void ssao::makenoisetexture(){
        if (noisetex){
            glDeleteTextures(1, &noisetex);
        }
        printf("%s", glm::to_string(noise[0]).c_str());
        glCreateTextures(GL_TEXTURE_2D, 1, &noisetex);
        glBindTexture(GL_TEXTURE_2D, noisetex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, glm::value_ptr(noise[0]));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        en::Log::Info("in makenoisetexture:");
    }
    void ssao::makeblurfbo(int width, int height) {
        if (width == 0 || height == 0)
            return;

        if (blurfbo) {
            glDeleteFramebuffers(1, &blurfbo);
        }
        if (blurtex){
            glDeleteTextures(1, &blurtex);
        }
        glCreateTextures(GL_TEXTURE_2D, 1, &blurtex);
        glTextureStorage2D(blurtex, 1, GL_R32F, width, height);
        glCreateFramebuffers(1, &blurfbo);
        glNamedFramebufferTexture(blurfbo, GL_COLOR_ATTACHMENT0, blurtex, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, blurfbo);
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glNamedFramebufferDrawBuffers(blurfbo, 1, DrawBuffers);
        if (glCheckNamedFramebufferStatus(blurfbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            printf("Incomplete FBO!");
            std::terminate();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    void ssao::makessaofbo(int width, int height) {
        if (width == 0 || height == 0)
            return;

        if (ssaofbo) {
            glDeleteFramebuffers(1, &ssaofbo);
        }
        if (ssaotex){
            glDeleteTextures(1, &ssaotex);
        }
        glCreateTextures(GL_TEXTURE_2D, 1, &ssaotex);
        glTextureStorage2D(ssaotex, 1, GL_R32F, width, height);
        glCreateFramebuffers(1, &ssaofbo);
        glNamedFramebufferTexture(ssaofbo, GL_COLOR_ATTACHMENT0, ssaotex, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaofbo);
        GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
        glNamedFramebufferDrawBuffers(ssaofbo, 1, DrawBuffers);
        if (glCheckNamedFramebufferStatus(ssaofbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            printf("Incomplete FBO!");
            std::terminate();
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    unsigned int ssao::setup_fullscreen_quad() {
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
    const GLProgram* ssao::makessaoprogram() {
        const en::GLShader* vertShader = GLShader::Load("ssao.vert");
        const en::GLShader* fragShader= GLShader::Load("ssao.frag");
        //en::GLProgram program(&vertShader, nullptr, &fragShader);
        //en::Log::Info("in ssaoprog");
        return GLProgram::Load(vertShader, nullptr, fragShader);
    }
    const GLProgram* ssao::makeblurprogram() {
       const en::GLShader* vertShader= GLShader::Load("ssao.vert");
       const  en::GLShader* fragShader= GLShader::Load("ssaoblur.frag");
        //en::GLProgram program(&vertShader, nullptr, &fragShader);
        en::Log::Info("in ssaoblurprog");
        return GLProgram::Load(vertShader, nullptr, fragShader);
    }
    ssao::ssao(int width, int height) {
        makekernels();
        makenoise();
        makenoisetexture();
        makessaofbo(width, height);
        makeblurfbo(width, height);
        quad = setup_fullscreen_quad();
    }
    void ssao::dossao(const GLProgram *ssaoprog, const GLProgram *blurprog, const GBuffer* buffer, const Camera* cam) const {
        //ssaopass
        if (glCheckNamedFramebufferStatus(ssaofbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            printf("Incomplete FBO!");
            std::terminate();
        }
        if (glCheckNamedFramebufferStatus(blurfbo, GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            printf("Incomplete FBO!");
            std::terminate();
        }
        glClear(GL_COLOR_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaofbo);
        ssaoprog->Use();
        glBindVertexArray(quad);
        buffer->UseTexturesSSAO(ssaoprog);
        glBindTextureUnit(2, noisetex);
        ssaoprog->SetUniformI("noisetex", 2);
        ssaoprog->SetUniformMat4("viewprojmat", false, glm::value_ptr(cam->GetViewProjMat()));
        ssaoprog->SetUniformVec3f("campos", cam->GetPos());
        ssaoprog->SetUniformVec3f("camdir", cam->GetViewDir());
        for (int i = 0; i < kernelsize; ++i) {
            assert(kernelsize==ssao::kernel.size());
            ssaoprog->SetUniformVec3f("kernel[" + std::to_string(i) + "]", ssao::kernel[i]);
        };
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*) 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        //ssaoblurpass
        glClear(GL_COLOR_BUFFER_BIT);
        glBindFramebuffer(GL_FRAMEBUFFER, blurfbo);
        blurprog->Use();
        glBindVertexArray(quad);
        glBindTextureUnit(0, ssaotex);
        ssaoprog->SetUniformI("ssaores", 0);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*) 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //en::Log::Info("in ssaoprog");
    }
    void ssao::usessaotex(const GLProgram *program) const {
        glBindTextureUnit(30, blurtex);
        program->SetUniformI("ssao", 30);
    }
}