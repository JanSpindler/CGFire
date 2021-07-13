//
// Created by JS on 30/05/2021.
//

#ifndef CGFIRE_SIMPLEPOINTLIGHT_H
#define CGFIRE_SIMPLEPOINTLIGHT_H

#include "engine/render/Light.hpp"
#include "engine/model/Model.hpp"
#include "engine/render/Renderable.hpp"

namespace en
{
    class SimplePointLight : public PointLight, public Model
    {
    public:
        SimplePointLight(glm::vec3 color, float strength, uint32_t width, uint32_t height, const std::string& name);

        glm::vec3 GetPos() const override;
        glm::vec3 GetColor() const override;

    private:
        glm::vec3 color_;
    };
}

#endif //CGFIRE_SIMPLEPOINTLIGHT_H
