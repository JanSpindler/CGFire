//
// Created by JS on 16/06/2021.
//

#ifndef CGFIRE_3DSPLINE_HPP
#define CGFIRE_3DSPLINE_HPP

#include <vector>
#include "render/Renderable.hpp"
#include <glm/glm.hpp>

namespace en
{
    class Spline3D
    {
    public:
        Spline3D(const std::vector<glm::vec3>& controlPoints, bool loop);

        glm::vec3 GetPoint(float x) const;
        unsigned int GetControlPointCount() const;
        const std::vector<glm::vec3>& GetControlPoints() const;
        unsigned int GetPointCount() const;
        const std::vector<glm::vec3>& GetPoints() const;
        bool IsLooped() const;

    private:
        std::vector<glm::vec3> controlPoints_;
        std::vector<glm::vec3> points_;
        bool loop_;
    };

    class Spline3DRenderable : public Renderable
    {
    public:
        Spline3DRenderable(const Spline3D* spline);

        void Render(const GLProgram* program) const;

    private:
        const Spline3D* spline_;
        unsigned int lineVao_;
        unsigned int pointVao_;
    };
}

#endif //CGFIRE_3DSPLINE_HPP
