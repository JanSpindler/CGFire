//
// Created by JS on 28/05/2021.
//

#ifndef CGFIRE_QUAD_HPP
#define CGFIRE_QUAD_HPP

#include <glm/glm.hpp>
#include "engine/GLShader.hpp"
#include "engine/GLTexture.hpp"
#include "engine/Renderable.hpp"

namespace en
{
    class Quad : public Renderable
    {
    public:
        Quad();

        void Draw();

        void OnRender(glm::mat4& view_proj_matrix, glm::mat4& transform, glm::vec4& color);

    private:
        unsigned int vao_ = 0;
        const GLProgram* program_;
        const GLPictureTex* tex_;

        const GLfloat vertices[20] = {-1, -1, 0,    0, 0,
                                      -1,  1, 0,    0, 1,
                                      1,  1, 0,     1, 1,
                                      1, -1, 0,     1, 0};

        const GLuint indices[6] = {
                0, 1, 2, 0, 2, 3
        };
    };
}

#endif //CGFIRE_QUAD_HPP
