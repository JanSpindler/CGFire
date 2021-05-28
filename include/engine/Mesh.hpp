//
// Created by JS on 23/05/2021.
//

#ifndef CGFIRE_MESH_HPP
#define CGFIRE_MESH_HPP

#include <glm/glm.hpp>
#include <vector>

#include "engine/GLBuffer.hpp"

namespace en
{
    class Mesh
    {
    public:
        static std::vector<Mesh> LoadScene(const char* filename, bool smooth, glm::vec4 preColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

        Mesh(const char* filename, bool smooth, glm::vec4 preColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
        ~Mesh();

        void Bind() const;
        void Release() const;
        void Delete() const;

    private:
        unsigned int vao_;
        GLBuffer vbo_ = GLBuffer::INVALID;
        GLBuffer ibo_ = GLBuffer::INVALID;
        glm::mat4 transform_;
        unsigned int vertexCount_;
        std::vector<glm::vec3> positions_;
        std::vector<glm::vec3> normals_;
        std::vector<glm::vec4> colors_;
        std::vector<glm::uvec3> faces_;

        Mesh();
    };
}

#endif //CGFIRE_MESH_HPP
