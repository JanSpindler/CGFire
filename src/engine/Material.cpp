//
// Created by JS on 26/05/2021.
//

#include "engine/gr_include.hpp"
#include "engine/Material.hpp"
#include "engine/Util.hpp"

namespace en
{
    Material::Material(
            float shininess,
            glm::vec4 diffuseColor,
            glm::vec4 specularColor,
            const GLPictureTex* tex)
    {
        shininess_ = shininess;
        diffuseColor_ = diffuseColor;
        specularColor_ = specularColor;
        tex_ = tex;
    }

    Material::~Material()
    {
    }

    void Material::Use(const GLProgram* program) const
    {
        program->SetUniformF("mat_shininess", shininess_);
        program->SetUniformVec4f("mat_diffuse_color", diffuseColor_);
        program->SetUniformVec4f("mat_specular_color", specularColor_);
        program->SetUniformB("mat_use_tex", tex_ != nullptr);
        glActiveTexture(GL_TEXTURE0);
        program->SetUniformI("mat_tex", 0);
    }
}
