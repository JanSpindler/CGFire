//
// Created by JS on 24/05/2021.
//

#ifndef CGFIRE_CAMERA_HPP
#define CGFIRE_CAMERA_HPP

#include <glm/glm.hpp>
#include "Spline3D.hpp"

namespace en
{
    class Camera
    {
    public:
        Camera(glm::vec3 pos, glm::vec3 viewDir, glm::vec3 up, float aspectRatio, float fov, float near, float far);
        ~Camera();

        glm::mat4 GetViewMat() const;
        glm::mat4 GetProjMat() const;
        glm::mat4 GetViewProjMat() const;

        const glm::vec3& GetPos() const;
        const glm::vec3& GetViewDir() const;

        void SetPos(glm::vec3 pos);
        void SetViewDir(glm::vec3 viewDir);
        void SetUp(glm::vec3 up);
        void SetAspectRatio(float aspectRatio);
        void SetFov(float fov);
        void SetNear(float near);
        void SetFar(float far);

        void Move(glm::vec3 movement);
        void RotateViewDir(float phi, float theta);

        void TrackSpline(const Spline3D* spline, float trackSpeed);
        void TrackStep(float deltaTime, glm::vec3 center);

    private:
        const Spline3D* spline_;
        float trackSpeed_;
        Spline3DIterator* iterator_;

        glm::vec3 pos_;
        glm::vec3 viewDir_;
        glm::vec3 up_;
        float aspectRatio_;
        float fov_;
        float near_;
        float far_;
    };
}

#endif //CGFIRE_CAMERA_HPP
