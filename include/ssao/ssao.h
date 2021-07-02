//
// Created by Nika on 28/06/2021.
//

#include <vector>
#include <glm/vec3.hpp>
#include <glm/glm.hpp>
#include "util/Random.h"
//#include <GLFW/glfw3.h>
#include "engine/render/GLShader.hpp"
#include "engine/render/Renderable.hpp"
#include "engine/render/GBuffer.hpp"
#ifndef CGFIRE_SSAO_H
#define CGFIRE_SSAO_H
#define kernelsize 64

namespace en {
    class ssao {
    public:
        ssao(int width, int height);
        void makessaofbo(int width, int height);
        void makeblurfbo(int width, int height);
        void usessaotex(const GLProgram* program) const;
        void dossao(const GLProgram* ssaoprog,const GLProgram* blurprog, const GBuffer* buffer, glm::mat4 ProjMat) const;
        const GLProgram* makessaoprogram();
        const GLProgram* makeblurprogram();
        std::vector<glm::vec3> kernel;
        std::vector<glm::vec3> noise;
        unsigned int noisetex=0;
        unsigned int ssaofbo=0;
        unsigned int blurfbo=0;
        unsigned int blurtex=0;
        unsigned int ssaotex=0;
        unsigned int quad=0;
    private:
        void makekernels();
        void makenoise();
        void makenoisetexture();
        static unsigned int setup_fullscreen_quad();
    };
}


#endif //CGFIRE_SSAO_H
