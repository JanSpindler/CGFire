//
// Created by JS on 23/05/2021.
//

#ifndef CGFIRE_MESH_HPP
#define CGFIRE_MESH_HPP

#include <glm/glm.hpp>
#include <vector>

namespace en
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    class Mesh
    {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        ~Mesh();

        void Draw() const;
        void Bind() const;
        void Delete() const;

    private:
        unsigned int vao_;
        unsigned int vbo_;
        unsigned int ibo_;
        std::vector<Vertex> vertices_;
        std::vector<unsigned int> indices_;
    };
}

#endif //CGFIRE_MESH_HPP
