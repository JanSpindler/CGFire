//
// Created by JS on 28/05/2021.
//

#include "engine/gr_include.hpp"
#include "engine/Quad.hpp"

namespace en
{
    Quad::Quad()
    {
        glCreateVertexArrays(1, &m_VA);
        glBindVertexArray(m_VA);

        GLuint vb, ib;
        glCreateBuffers(1, &vb);

        glBindBuffer(GL_ARRAY_BUFFER, vb);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexArrayAttrib(vb, 0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));

        glEnableVertexArrayAttrib(vb, 1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glCreateBuffers(1, &ib);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        // load and compile shaders and link program
        unsigned int vertexShader = compileShader("CGFire/particles.vert", GL_VERTEX_SHADER);
        unsigned int fragmentShader = compileShader("CGFire/particles.frag", GL_FRAGMENT_SHADER);
        m_Shader = linkProgram(vertexShader, fragmentShader);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);

        // get uniform locations
        m_ShaderViewProj = glGetUniformLocation(m_Shader, "u_ViewProj");
        m_ShaderTransform = glGetUniformLocation(m_Shader, "u_Transform");
        m_ShaderColor = glGetUniformLocation(m_Shader, "u_Color");
        m_ShaderTexture = glGetUniformLocation(m_Shader, "u_Texture");

        //load texture
        int width, height;
        //TODO: load Funktion soll auch Alpha Channel laden
        float* data = util::load_texture_data(DATA_ROOT + "CGFire/spark2.png", &width, &height);

        glCreateTextures(GL_TEXTURE_2D, 1, &m_ShaderTexture);
        glTextureStorage2D(m_ShaderTexture, 1, GL_RGBA32F, width, height);
        glTextureSubImage2D(m_ShaderTexture, 0, 0, 0, width, height, GL_RGBA, GL_FLOAT, data);
        glBindTextureUnit(0, m_ShaderTexture);

        delete [] data;
    }

    void Quad::OnRender(glm::mat4& view_proj_matrix, glm::mat4& transform, glm::vec4& color)
    {

    }
}
