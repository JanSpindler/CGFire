//
// Created by JS on 23/05/2021.
//

#include "engine/gr_include.hpp"

#include <glm/gtx/hash.hpp>

#include "engine/Mesh.hpp"

namespace std
{
    size_t hash<en::Vertex>::operator()(const en::Vertex &vert) const
    {
        size_t h1 = hash<glm::vec3>()(vert.pos);
        size_t h2 = hash<glm::vec3>()(vert.normal);
        size_t h3 = hash<glm::vec2>()(vert.uv);
        return ((h1 ^ (h2 << 1)) >> 1) ^ h3;
    }
}

namespace en
{
    bool Vertex::operator==(const Vertex &other) const
    {
        return (pos == other.pos) && (normal == other.normal) && (uv == other.uv);
    }

    Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    {
        vertices_ = vertices;
        indices_ = indices;

        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(en::Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &ibo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(en::Vertex), (void*)offsetof(en::Vertex, en::Vertex::pos));
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(en::Vertex), (void*)offsetof(en::Vertex, en::Vertex::normal));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(en::Vertex), (void*)offsetof(en::Vertex, en::Vertex::uv));
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::Draw() const
    {
        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
    }

    void Mesh::Bind() const
    {
        glBindVertexArray(vao_);
    }

    void Mesh::Delete() const
    {
        glDeleteVertexArrays(1, &vao_);
        glDeleteBuffers(1, &vbo_);
        glDeleteBuffers(1, &ibo_);
    }
}
