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
                const GLPictureTex* diffuseTex,
                const GLPictureTex* specularTex,
                glm::vec4 diffuseColor,
                glm::vec4 specularColor,
                float shininess
                );
        ~Material();

        void UseDiffuse(const GLProgram* program, uint32_t diffTexUnit) const;
        void UseAll(const GLProgram* program, uint32_t diffTexUnit, uint32_t specTexUnit) const;

        glm::vec4 GetDiffuseColor() const;

    private:
        const GLPictureTex* diffuseTex_;
        const GLPictureTex* specularTex_;
        glm::vec4 diffuseColor_;
        glm::vec4 specularColor_;
        float shininess_;
    };
}

#endif //CGFIRE_MATERIAL_HPP
