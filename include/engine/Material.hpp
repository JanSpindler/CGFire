//
// Created by JS on 26/05/2021.
//

#ifndef CGFIRE_MATERIAL_HPP
#define CGFIRE_MATERIAL_HPP

#include "GLTexture.hpp"
#include <glm/glm.hpp>
#include "GLShader.hpp"

namespace en
{
    class Material
    {
    public:
        Material(
                float shininess,
                glm::vec3 diffuseColor,
                glm::vec3 specularColor,
                const GLPictureTex* tex);
        ~Material();

        void Apply(const GLProgram* program) const;

    private:
        float shininess_;
        glm::vec3 diffuseColor_;
        glm::vec3 specularColor_;
        const GLPictureTex* tex_;
    };
}

#endif //CGFIRE_MATERIAL_HPP
