//
// Created by JS on 24/05/2021.
//

#include "engine/Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace en
{
    Camera::Camera(glm::vec3 pos, glm::vec3 viewDir, glm::vec3 up, float aspectRatio, float fov, float near, float far) :
        pos_(pos),
        viewDir_(glm::normalize(viewDir)),
        up_(up),
        aspectRatio_(aspectRatio),
        near_(near),
        far_(far),
        spline_(nullptr),
        iterator_(Spline3D::Iterator(0, 0.0f))
    {
    }

    Camera::~Camera()
    {
    }

    glm::mat4 Camera::GetViewMat() const
    {
        return glm::lookAt(pos_, pos_ + viewDir_, up_);
    }

    glm::mat4 Camera::GetProjMat() const
    {
        return glm::perspective(fov_, aspectRatio_, near_, far_);
    }
    glm::mat4 Camera::GetViewProjMat() const{
        return this->GetProjMat() * this->GetViewMat();;
    }

    glm::vec3 Camera::GetPos() const
    {
        return pos_;
    }

    glm::vec3 Camera::GetViewDir() const
    {
        return viewDir_;
    }

    void Camera::SetPos(glm::vec3 pos)
    {
        if (spline_ != nullptr)
            spline_ = nullptr;
        pos_ = pos;
    }

    void Camera::SetViewDir(glm::vec3 viewDir)
    {
        if (spline_ != nullptr)
            spline_ = nullptr;
        viewDir_ = glm::normalize(viewDir);
    }

    void Camera::SetUp(glm::vec3 up)
    {
        up_ = glm::normalize(up);
    }

    void Camera::SetAspectRatio(float aspectRatio)
    {
        aspectRatio_ = aspectRatio;
    }

    void Camera::SetFov(float fov)
    {
        fov_ = fov;
    }

    void Camera::SetNear(float near)
    {
        near_ = near;
    }

    void Camera::SetFar(float far)
    {
        far_ = far;
    }

    void Camera::Move(glm::vec3 movement)
    {
        if (movement == glm::vec3(0.0f))
            return;

        if (spline_ != nullptr)
            spline_ = nullptr;

        glm::vec3 frontMove = viewDir_ * movement.z;
        glm::vec3 leftMove = glm::normalize(glm::cross(viewDir_, up_)) * movement.x;
        glm::vec3 upMove = up_ * movement.y;

        pos_ += frontMove + leftMove + upMove;
    }

    void Camera::RotateViewDir(float phi, float theta)
    {
        if (phi == 0.0f && theta == 0.0f)
            return;

        if (spline_ != nullptr)
            spline_ = nullptr;

        glm::vec3 phiAxis = up_;
        glm::mat3 phiMat = glm::rotate(glm::identity<glm::mat4>(), phi, phiAxis);

        glm::vec3 thetaAxis = glm::normalize(glm::cross(viewDir_, up_));
        glm::mat3 thetaMat = glm::rotate(glm::identity<glm::mat4>(), theta, thetaAxis);

        viewDir_ = glm::normalize(thetaMat * phiMat * viewDir_);
    }

    void Camera::TrackSpline(const Spline3D* spline)
    {
        spline_ = spline;
        iterator_ = Spline3D::Iterator(0, 0.0f);
    }

    void Camera::TrackStep(float t, glm::vec3 center)
    {
        if (spline_ == nullptr)
            return;

        pos_ = spline_->IterateRelative(&iterator_, t);
        viewDir_ = glm::normalize(center - pos_);
    }
}
