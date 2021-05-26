//
// Created by JS on 26/05/2021.
//

#include "engine/gr_include.hpp"
#include "engine/Material.hpp"

namespace en
{
    Material::Material(
            float shininess,
            glm::vec3 diffuseColor,
            glm::vec3 specularColor,
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

    void Material::Apply(const GLProgram* program) const
    {
        program->SetUniformF("shininess", shininess_);
        program->SetUniformVec3f("diffuse_color", diffuseColor_);
        program->SetUniformVec3f("specular_color", specularColor_);
        program->SetUniformB("use_tex", tex_ != nullptr);
        glActiveTexture(GL_TEXTURE0);
        program->SetUniformI("tex", 0);
    }
}
