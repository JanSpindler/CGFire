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
    struct Spline3DIterator
    {
        uint32_t lastPoint;
        float lastInterp;

        Spline3DIterator(uint32_t lP, float lI);
    };

    class Spline3D : public RenderObj
    {
    public:
        static const uint8_t TYPE_CATMULL_ROM = 0;
        static const uint8_t TYPE_NATURAL_CUBIC = 1;

        Spline3D(const std::vector<glm::vec3>& controlPoints, bool loop, uint32_t resolution, uint8_t type);
        void Recreate(const std::vector<glm::vec3>& controlPoints);

        void Render(const GLProgram* program); // Deprecated
        void RenderPosOnly(const GLProgram* program);
        void RenderDiffuse(const GLProgram* program);
        void RenderAll(const GLProgram* program);
        void OnImGuiRender();

        glm::vec3 IterateRelative(Spline3DIterator* iterator, float t) const;
        glm::vec3 IterateAbsolute(float t) const;
        glm::vec3 GetSegmentDir(uint32_t segmentIndex) const;

        unsigned int GetControlPointCount() const;
        const std::vector<glm::vec3>& GetControlPoints() const;

        unsigned int GetPointCount() const;
        const std::vector<glm::vec3>& GetPoints() const;

        bool IsLooped() const;
        float GetTotalLength() const;

    private:
        uint32_t pointVao_;
        uint32_t lineVao_;

        std::vector<glm::vec3> controlPoints_;
        std::vector<glm::vec3> points_;
        bool loop_;
        uint32_t resolution_;
        uint8_t type_;
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

        void GenVao();
        void RenderPoints() const;
        void RenderLines() const;
    };
}

#endif //CGFIRE_3DSPLINE_HPP
