//
// Created by JS on 23/05/2021.
//

#ifndef CGFIRE_MESH_HPP
#define CGFIRE_MESH_HPP

#include <glm/glm.hpp>
#include <vector>
#include <string>

#include "engine/Render/GLShader.hpp"
#include "Vertex.hpp"
#include "engine/Render/GLTexture.hpp"
#include "engine/Render/Material.hpp"
#include "engine/Render/Renderable.hpp"

namespace en
{
    class Mesh : public Renderable
    {
    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, const Material* material);
        ~Mesh();

        void Render(const GLProgram* program) const override;
        void RenderGeometry(const GLProgram* program) const override;

    private:
        unsigned int vao_;
        std::vector<Vertex> vertices_;
        std::vector<unsigned int> indices_;
        const Material* material_;
    };
}

#endif //CGFIRE_MESH_HPP
