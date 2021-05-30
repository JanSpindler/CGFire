//
// Created by JS on 30/05/2021.
//

#ifndef CGFIRE_CRSPLINE_HPP
#define CGFIRE_CRSPLINE_HPP

#include <vector>
#include <glm/glm.hpp>

namespace en
{
    // Catmull-Rom-Spline
    class CRSpline
    {
    public:
        CRSpline(const std::vector<glm::vec3>& controlPoints, bool loop, unsigned int cachePerPair, float alpha);

        glm::vec3 GetPoint(double t);
        glm::vec3 GetPoint(unsigned int i, float t);

    private:
        std::vector<glm::vec3> controlPoints_;
        std::vector<glm::vec3> cachedPoints_;

        static float GetT(float t, float alpha, glm::vec3 p0, glm::vec3 p1);
        static glm::vec3 GetPoint(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t, float alpha);
    };
}

#endif //CGFIRE_CRSPLINE_HPP
