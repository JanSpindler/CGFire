//
// Created by JS on 30/05/2021.
//

#include <engine/CRSpline.hpp>
#include <glm/gtx/compatibility.hpp>
#include "engine/Util.hpp"

namespace en
{
    CRSpline::CRSpline(const std::vector<glm::vec3>& controlPoints, bool loop, unsigned int cachePerPair, float alpha)
    {
        controlPoints_ = controlPoints;

        if (cachePerPair < 1)
            Log::Error("cachePerPair must be >= 1", true);

        unsigned int pointCount = controlPoints.size();
        if (pointCount < 2)
            Log::Error("CRSpline should at least contain 2 points", true);

        unsigned int pairCount = pointCount - 1;
        unsigned int cacheSize = pairCount * cachePerPair;
        cachedPoints_.resize(cacheSize);

        glm::vec3 pbf; // point before first point
        glm::vec3 pal; // point after last point
        if (loop)
        {
            pbf = controlPoints[pointCount - 1];
            pal = controlPoints[0];
        }
        else
        {
            pbf = controlPoints[0] - (controlPoints[1] - controlPoints[0]);
            pal = controlPoints[pointCount - 1] + (controlPoints[pointCount - 1] - controlPoints[pointCount - 2]);
        }

        glm::vec3 point0, point1, point2, point3;
        unsigned int index = 0;
        float dt = 1.0f / (float) cachePerPair;
        for (unsigned int j = 0; j < pairCount; j++)
        {
            point1 = controlPoints[j];
            point2 = controlPoints[j + 1];

            if (j == 0)
                point0 = pbf;
            else
                point0 = controlPoints[j - 1];

            if (j == pairCount - 1)
                point3 = pal;
            else
                point3 = controlPoints[j + 2];

            for (unsigned int i = 0; i < cachePerPair; i++)
                cachedPoints_[index++] = GetPoint(point0, point1, point2, point3, dt * (float)i, alpha);
        }
    }

    glm::vec3 CRSpline::GetPoint(double t)
    {
        double di = (double) cachedPoints_.size() * t;
        unsigned int i = (unsigned int) di;
        // TODO: handle t == 1.0
        double rt = di - (double) i;
        return GetPoint(i, (float)rt);
    }

    glm::vec3 CRSpline::GetPoint(unsigned int i, float t)
    {
        glm::vec3 p0 = cachedPoints_[i];
        glm::vec3 p1 = cachedPoints_[i + 1];
        return glm::lerp(p0, p1, t);
    }

    float CRSpline::GetT(float t, float alpha, glm::vec3 p0, glm::vec3 p1)
    {
        glm::vec3 d = p1 - p0;
        float a = glm::dot(d, d);
        float b = glm::pow(a, alpha * 0.5f);
        return b + t;
    }

    glm::vec3 CRSpline::GetPoint(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t, float alpha)
    {
        float t0 = 0.0f;
        float t1 = GetT(t0, alpha, p0, p1);
        float t2 = GetT(t1, alpha, p1, p2);
        float t3 = GetT(t2, alpha, p2, p3);
        t = glm::lerp(t1, t2, t);
        glm::vec3 a1 = (t1 - t) / (t1 - t0) * p0 + (t - t0) / (t1 - t0) * p1;
        glm::vec3 a2 = (t2 - t) / (t2 - t1) * p1 + (t - t1) / (t2 - t1) * p2;
        glm::vec3 a3 = (t3 - t) / (t3 - t2) * p2 + (t - t2) / (t3 - t2) * p3;
        glm::vec3 b1 = (t2 - t) / (t2 - t0) * a1 + (t - t0) / (t2 - t0) * a2;
        glm::vec3 b2 = (t3 - t) / (t3 - t1) * a2 + (t - t1) / (t3 - t1) * a3;
        glm::vec3 c  = (t2 - t) / (t2 - t1) * a1 + (t - t1) / (t2 - t1) * b2;
        return c;
    }
}
