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
        static const uint8_t TYPE_CATMULL_ROM = 0;
        static const uint8_t TYPE_NATURAL_CUBIC = 1;

        Spline3D(const std::vector<glm::vec3>& controlPoints, bool loop, uint32_t resolution, uint8_t type);

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

        void ConstructCatmullRom(uint32_t resolution);
        static glm::vec3 CatmullRom(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t);

        void ConstructNaturalCubic(uint32_t resolution);
        static std::vector<float> CalcNaturalCubicMoments(const std::vector<float>& h, const std::vector<float>& y, bool loop);
        static std::vector<glm::vec4> CalcNaturalCubicCoef(
                const std::vector<float>& h,
                const std::vector<float>& y,
                const std::vector<float>& m);
        static float NaturalCubic(glm::vec4 coef, float x);
    };

    class Spline3DRenderable : public Renderable
    {
    public:
        Spline3DRenderable(const Spline3D* spline);

        void Render(const GLProgram* program) const override;
        void RenderToGBuffer(const GLProgram* program) const override;
        void RenderToShadowMap(const GLProgram* program) const override;
        void RenderFixedColor(const GLProgram* program) const override;

    private:
        const Spline3D* spline_;
        unsigned int lineVao_;
        unsigned int pointVao_;

        void RenderLines() const;
        void RenderPoints() const;
    };
}

#endif //CGFIRE_3DSPLINE_HPP
