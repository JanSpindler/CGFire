//
// Created by JS on 28/06/2021.
//

#ifndef CGFIRE_REFLECTIVEMAP_HPP
#define CGFIRE_REFLECTIVEMAP_HPP

#include <vector>
#include <glm/glm.hpp>

namespace en
{
    class ReflectiveMap
    {
    public:
        ReflectiveMap(uint32_t size);
        ~ReflectiveMap();

        void BindFbo() const;
        void UnbindFbo() const;

        void BindCubeMap() const;
        void BindCubeMapFace(uint8_t i) const;

        uint32_t GetSize() const;
        glm::mat4 GetProjMat() const;
        std::vector<glm::mat4> GetViewMats(glm::vec3 pos) const;

    private:
        uint32_t fbo_;
        uint32_t cubeMap_;
        uint32_t depthBuffer_;

        uint32_t size_;
    };
}

#endif //CGFIRE_REFLECTIVEMAP_HPP
