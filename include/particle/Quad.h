//
//Created by vincent on 26.05.2021.
//

#pragma once

#include <glad/glad.h>
#include "util/TextureLoading.h"

#include "framework/shader.hpp"


//An OpenGL drawable quad used as a particle
class Quad {
    public:
    Quad();

    void OnRender(glm::mat4& view_proj_matrix, glm::mat4& transform, glm::vec4& color);

    private:

    GLuint m_VA = 0;
    GLuint m_Shader;
    GLint m_ShaderViewProj;
    GLint m_ShaderTransform;
    GLint m_ShaderColor;
    GLuint m_ShaderTexture;


    const GLfloat vertices[20] = {-1, -1, 0,    0, 0,
                            -1,  1, 0,    0, 1,
                            1,  1, 0,     1, 1,
                            1, -1, 0,     1, 0};


    const GLuint indices[6] = {
            0, 1, 2, 0, 2, 3
    };
};

