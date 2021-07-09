//
// Created by JS on 26/05/2021.
//
//19/06/21 tex_.bind() eingefügt von Nika, sonst laufen multiple renderpasses nicht wenn man bei anderem programm was anderes dran bindet
//and erased again, since it was added in engine update

#include "engine/gr_include.hpp"
#include "engine/Render/Material.hpp"
#include "engine/Util.hpp"

namespace en
{
    Material::Material(
            const GLPictureTex* diffuseTex,
            const GLPictureTex* specularTex,
            const GLPictureTex* normalTex,
            glm::vec4 diffuseColor,
            glm::vec4 specularColor,
            float shininess) :
            diffuseTex_(diffuseTex),
            specularTex_(specularTex),
            normalTex_(normalTex),
            diffuseColor_(diffuseColor),
            specularColor_(specularColor),
            shininess_(shininess)
    {
    }

    Material::~Material()
    {
    }

    void Material::UseDiffuse(const GLProgram *program, uint32_t diffTexUnit) const
    {
        bool useDiffTex = diffuseTex_ != nullptr;
        program->SetUniformB("use_diffuse_tex", useDiffTex);

        glActiveTexture(GL_TEXTURE0 + diffTexUnit);
        if (useDiffTex)
            diffuseTex_->BindTex();
        program->SetUniformI("diffuse_tex", diffTexUnit);

        program->SetUniformVec4f("diffuse_color", diffuseColor_);
    }

    void Material::UseAll(const GLProgram *program, uint32_t diffTexUnit, uint32_t specTexUnit, uint32_t normalTexUnit) const
    {
        // Diffuse
        bool useDiffTex = diffuseTex_ != nullptr;
        program->SetUniformB("use_diffuse_tex", useDiffTex);

        glActiveTexture(GL_TEXTURE0 + diffTexUnit);
        if (useDiffTex)
            diffuseTex_->BindTex();
        program->SetUniformI("diffuse_tex", diffTexUnit);

        program->SetUniformVec4f("diffuse_color", diffuseColor_);

        // Specular
        bool useSpecTex = specularTex_ != nullptr;
        program->SetUniformB("use_specular_tex", useSpecTex);

        glActiveTexture(GL_TEXTURE0 + specTexUnit);
        if (useSpecTex)
            specularTex_->BindTex();
        program->SetUniformI("specular_tex", specTexUnit);

        program->SetUniformVec4f("specular_color", specularColor_);

        // Normal
        bool useNormalTex = normalTex_ != nullptr;
        program->SetUniformB("use_normal_tex", useNormalTex);

        glActiveTexture(GL_TEXTURE0 + normalTexUnit);
        if (useNormalTex)
            normalTex_->BindTex();
        program->SetUniformI("normal_tex", normalTexUnit);

        // Shininess
        program->SetUniformF("shininess", shininess_);
    }

    glm::vec4 Material::GetDiffuseColor() const
    {
        return diffuseColor_;
    }
}
