//
// Created by Nika on 28/06/2021.
//
//uses setup_fullscreen_quad from aufgabe7
//based on https://learnopengl.com/Advanced-Lighting/SSAO, https://john-chapman-graphics.blogspot.com/2013/01/ssao-tutorial.html
#include <cmath>
#include "ssao/ssao.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "framework/buffer.hpp"


namespace en{
    void ssao::makekernels() {
        for (int i=0; i<kernelsize; i++){
            glm::vec3 currentkernel (
                    float() *2-1,
                    float()*2-1,
                    float()
                    );
            currentkernel = glm::normalize(currentkernel);
            currentkernel *= float();
            float scale = float(i)/float(kernelsize);
            scale = 0.1f + (1.0f-0.1f)*(scale*scale);
            currentkernel *= scale;
            ssao::kernel.push_back(currentkernel);
        }
    }
    void ssao::makenoise() {
        for (int i=0; i<16; i++){
            glm::vec3 newnoise (
                    float()*2-1,
                    float()*2-1,
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
        glCreateTextures(GL_TEXTURE_2D, 1, &noisetex);
        glTexImage2D(noisetex, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &noise);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }
    void ssao::makeblurfbo(int width, int height) {
        if (blurfbo) {
            glDeleteFramebuffers(1, &blurfbo);
        }
        if (blurtex){
            glDeleteTextures(1, &blurtex);
        }
        glCreateTextures(GL_TEXTURE_2D, 1, &blurtex);
        glTextureStorage2D(blurtex, 1, GL_R, width, height);
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
        if (ssaofbo) {
            glDeleteFramebuffers(1, &ssaofbo);
        }
        if (ssaotex){
            glDeleteTextures(1, &ssaotex);
        }
        glCreateTextures(GL_TEXTURE_2D, 1, &ssaotex);
        glTextureStorage2D(ssaotex, 1, GL_R, width, height);
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
    GLProgram ssao::makessaoprogram() {
        en::GLShader vertShader("ssao.vert", en::GLShader::Type::VERTEX);
        en::GLShader fragShader("ssao.frag", en::GLShader::Type::FRAGMENT);
        en::GLProgram program(&vertShader, nullptr, &fragShader);
        return program;
    }
    GLProgram ssao::makeblurprogram() {
        en::GLShader vertShader("ssao.vert", en::GLShader::Type::VERTEX);
        en::GLShader fragShader("ssaoblur.frag", en::GLShader::Type::FRAGMENT);
        en::GLProgram program(&vertShader, nullptr, &fragShader);
        return program;
    }
    ssao::ssao(int width, int height) {
        makekernels();
        makenoise();
        makenoisetexture();
        makessaofbo(width, height);
        makeblurfbo(width, height);
        quad = setup_fullscreen_quad();
    }
}