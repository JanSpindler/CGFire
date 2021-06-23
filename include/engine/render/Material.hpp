//
// Created by JS on 26/05/2021.
//

#ifndef CGFIRE_MATERIAL_HPP
#define CGFIRE_MATERIAL_HPP

#include "engine/Render/GLTexture.hpp"
#include <glm/glm.hpp>
#include "engine/Render/GLShader.hpp"

namespace en
{
    class Material
    {
    public:
        Material(
                float shininess,
                glm::vec4 diffuseColor,
                glm::vec4 specularColor,
                const GLPictureTex* tex);
        ~Material();

        void Use(const GLProgram* program) const;
        void UseGeometry(const GLProgram* program) const;

    private:
        float shininess_;
        glm::vec4 diffuseColor_;
        glm::vec4 specularColor_;
        const GLPictureTex* tex_;
    };
}

#endif //CGFIRE_MATERIAL_HPP
