//
// Created by JS on 23/05/2021.
//

#ifndef CGFIRE_MESH_HPP
#define CGFIRE_MESH_HPP

#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "engine/GLShader.hpp"

namespace en
{
    struct Vertex
    {
        glm::vec3 pos;
        glm::vec3 normal;
        glm::vec2 uv;
    };

    struct Texture
    {
        unsigned int id;
        std::string type;
        std::string path;

        Texture(const std::string& path);
    };

    class Mesh
    {
    public:
        std::vector<Vertex> vertices_;
        std::vector<unsigned int> indices_;
        std::vector<Texture> textures_;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        ~Mesh();

        void Draw(const GLProgram* program) const;

    private:
        unsigned int vao_;
        unsigned int vbo_;
        unsigned int ibo_;

        void SetupMesh();
    };
}

#endif //CGFIRE_MESH_HPP
