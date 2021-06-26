//
// Created by JS on 23/05/2021.
//

#include "engine/gr_include.hpp"

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/gtx/hash.hpp>

#include "engine/Model/Mesh.hpp"
#include "engine/Util.hpp"

namespace en
{
    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const Material* material)
    {
        vertices_ = vertices;
        indices_ = indices;
        material_ = material;

        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        unsigned int vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STATIC_DRAW);

        unsigned int ibo;
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::pos_));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::normal_));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::uv_));

        glBindVertexArray(0);
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::Render(const GLProgram* program) const
    {
        material_->Use(program);
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, (GLsizei)indices_.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void Mesh::RenderToGBuffer(const GLProgram *program) const
    {
        material_->UseGeometry(program);
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void Mesh::RenderToShadowMap(const GLProgram *program) const
    {
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void Mesh::RenderFixedColor(const GLProgram *program) const
    {
        program->SetUniformVec4f("fixed_color", material_->GetDiffuseColor());
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
}
