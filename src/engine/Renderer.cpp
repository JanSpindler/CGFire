//
// Created by JS on 26/05/2021.
//

#include "engine/Renderer.hpp"

namespace en
{
    Renderer::Renderer(const GLProgram* program)
    {
        program_ = program;
    }

    Renderer::~Renderer()
    {
    }

    void Renderer::Render(const Model* model)
    {
        program_->Use();
        program_->SetUniformVec3f("dir_light_dir", dirLight_.dir_);
        program_->SetUniformVec3f("dir_light_color", dirLight_.color_);
        // TODO: set point light uniforms
        model->Draw(program_);
    }

    void Renderer::SetDirectionalLight(DirectionalLight light)
    {
        dirLight_ = light;
    }

    void Renderer::AddPointLight(PointLight light)
    {
        if (std::find(pointLights_.begin(), pointLights_.end(), light) != pointLights_.end())
            pointLights_.push_back(light);
    }

    void Renderer::RemovePointLight(PointLight light)
    {
        for (unsigned int i = 0; i < pointLights_.size(); i++)
        {
            if (pointLights_[i] == light)
                pointLights_.erase(pointLights_.begin() + i);
        }
    }
}
