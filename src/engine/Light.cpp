//
// Created by JS on 26/05/2021.
//

#include "engine/Render/Light.hpp"

namespace en
{
    DirLight::DirLight(glm::vec3 dir, glm::vec3 color)
    {
        dir_ = dir;
        color_ = color;
    }

    void DirLight::Use(const GLProgram *program) const
    {
        program->SetUniformVec3f("dir_light_dir", dir_);
        program->SetUniformVec3f("dir_light_color", color_);
    }

    PointLight::PointLight(glm::vec3 pos, glm::vec3 color)
    {
        pos_ = pos;
        color_ = color;
    }

    void PointLight::Use(const GLProgram *program, unsigned int index) const
    {
        std::string indexStr = std::to_string(index);
        program->SetUniformVec3f(std::string("point_light_pos[" + indexStr + "]").c_str(), pos_);
        program->SetUniformVec3f(std::string("point_light_color[" + indexStr + "]").c_str(), color_);
    }

    bool PointLight::operator==(const PointLight &other) const
    {
        return (pos_ == other.pos_) && (color_ == other.color_);
    }
}
