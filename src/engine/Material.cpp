//
// Created by JS on 26/05/2021.
//

#include "engine/gr_include.hpp"
#include "engine/Render/Material.hpp"
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
        if (tex_ != nullptr)
            tex_->BindTex();
        program->SetUniformI("mat_tex", 0);
    }

    void Material::UseGeometry(const GLProgram *program) const
    {
        // Diffuse
        bool useDiffTex = tex_ != nullptr;
        program->SetUniformB("use_diffuse_tex", useDiffTex);
        glActiveTexture(GL_TEXTURE0);
        if (useDiffTex)
            tex_->BindTex();
        program->SetUniformI("diffuse_tex", 0);
        program->SetUniformVec3f("diffuse_tex", glm::vec3(diffuseColor_));

        // Specular
        bool useSpecTex = false;//speculatTex_ != nullptr;
        program->SetUniformB("use_specular_tex", useSpecTex);
        glActiveTexture(GL_TEXTURE1);
        if (useSpecTex)
            //specularTex_->BindTex();
        program->SetUniformI("specular_tex", 1);
        program->SetUniformVec3f("specular_tex", glm::vec3(specularColor_));
    }
}
