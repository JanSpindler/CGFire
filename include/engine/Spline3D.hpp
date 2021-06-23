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
        Spline3D(const std::vector<glm::vec3>& controlPoints, bool loop, unsigned int resolution);

        glm::vec3 GetPoint(unsigned int pointIndex, float t) const;
        glm::vec3 GetPoint(float t) const;

        unsigned int GetControlPointCount() const;
        const std::vector<glm::vec3>& GetControlPoints() const;

        unsigned int GetPointCount() const;
        const std::vector<glm::vec3>& GetPoints() const;

        bool IsLooped() const;
        float GetTotalLength() const;

    private:
        std::vector<glm::vec3> controlPoints_;
        std::vector<glm::vec3> points_;
        bool loop_;
        float totalLength_;
        std::vector<float> segmentLengths_;

        void ConstructCatmullRom(unsigned int resolution);
        void ConstructNaturalCubic(unsigned int resolution);

        static glm::vec3 CatmullRom(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t);
        static std::vector<float> NaturalCubic(const std::vector<float>& x, const std::vector<float>& y);
    };

    class Spline3DRenderable : public Renderable
    {
    public:
        Spline3DRenderable(const Spline3D* spline);

        void Render(const GLProgram* program) const;
        void RenderGeometry(const GLProgram* program) const;

    private:
        const Spline3D* spline_;
        unsigned int lineVao_;
        unsigned int pointVao_;
    };
}

#endif //CGFIRE_3DSPLINE_HPP
