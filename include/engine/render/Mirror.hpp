//
// Created by JS on 30/06/2021.
//

#ifndef CGFIRE_MIRROR_HPP
#define CGFIRE_MIRROR_HPP

#include "Renderable.hpp"
#include "../Camera.hpp"

namespace en
{
    class Mirror
    {
    public:
        Mirror(const RenderObj* renderObj, glm::vec3 normal, uint32_t size);

        void BindFbo() const;
        void UnbindFbo() const;

        glm::mat4 GetViewMat(const Camera* cam) const;

    private:
        uint32_t fbo_;
        uint32_t colorTex_;
        uint32_t depthBuffer_;

        const RenderObj* renderObj_;
        glm::vec3 normal_;
    };
}

#endif //CGFIRE_MIRROR_HPP
