//
// Created by JS on 26/05/2021.
//

#include "engine/Light.hpp"

namespace en
{
    bool PointLight::operator==(const PointLight &other) const
    {
        return (pos_ == other.pos_) && (color_ == other.color_);
    }
}
