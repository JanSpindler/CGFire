//
// Created by JS on 26/05/2021.
//

#include "engine/gr_include.hpp"
#include "engine/Render/Material.hpp"
#include "engine/Util.hpp"

namespace en
{
    Material::Material(
            const GLPictureTex* diffuseTex,
            const GLPictureTex* specularTex,
            glm::vec4 diffuseColor,
            glm::vec4 specularColor,
            float shininess
            ) :
            diffuseTex_(diffuseTex),
            specularTex_(specularTex),
            diffuseColor_(diffuseColor),
            specularColor_(specularColor),
            shininess_(shininess)
    {
    }

    Material::~Material()
    {
    }

    void Material::Use(const GLProgram* program) const
    {
        program->SetUniformF("mat_shininess", shininess_);
        program->SetUniformVec4f("mat_diffuse_color", diffuseColor_);
        program->SetUniformVec4f("mat_specular_color", specularColor_);

        program->SetUniformB("mat_use_tex", diffuseTex_ != nullptr);
        glActiveTexture(GL_TEXTURE0);
        if (diffuseTex_ != nullptr)
            diffuseTex_->BindTex();
        program->SetUniformI("mat_tex", 0);
    }

    void Material::UseForGBuffer(const GLProgram *program) const
    {
        // Diffuse
        bool useDiffTex = diffuseTex_ != nullptr;
        program->SetUniformB("use_diffuse_tex", useDiffTex);
        glActiveTexture(GL_TEXTURE0);
        if (useDiffTex)
            diffuseTex_->BindTex();
        program->SetUniformI("diffuse_tex", 0);
        program->SetUniformVec3f("diffuse_color", glm::vec3(diffuseColor_));

        // Specular
        bool useSpecTex = specularTex_ != nullptr;
        program->SetUniformB("use_specular_tex", useSpecTex);
        glActiveTexture(GL_TEXTURE1);
        if (useSpecTex)
            specularTex_->BindTex();
        program->SetUniformI("specular_tex", 1);
        program->SetUniformVec3f("specular_color", glm::vec3(specularColor_));

        // Shininess
        program->SetUniformF("shininess", shininess_);
    }

    void Material::UseForSimpleDraw(const GLProgram *program) const
    {
        // Only use diffuse information
        bool useDiffTex = diffuseTex_ != nullptr;
        program->SetUniformB("use_diffuse_tex", useDiffTex);
        glActiveTexture(GL_TEXTURE0);
        if (useDiffTex)
            diffuseTex_->BindTex();
        program->SetUniformI("diffuse_tex", 0);
        program->SetUniformVec3f("diffuse_color", glm::vec3(diffuseColor_));
    }

    glm::vec4 Material::GetDiffuseColor() const
    {
        return diffuseColor_;
    }
}
