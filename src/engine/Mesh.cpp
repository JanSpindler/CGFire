//
// Created by JS on 23/05/2021.
//
//Annika added skeletal animation support 29/05/21, based on https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

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

        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::boneids_));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::boneweights_));


        glBindVertexArray(0);
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::RenderPosOnly(const GLProgram *program) const
    {
        GLDrawCall();
    }

    void Mesh::RenderDiffuse(const GLProgram *program) const
    {
        material_->UseDiffuse(program, 0);
        GLDrawCall();
    }

    void Mesh::RenderAll(const GLProgram *program) const
    {
        material_->UseAll(program, 0, 1);
        GLDrawCall();
    }

    void Mesh::GLDrawCall() const
    {
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }
}
