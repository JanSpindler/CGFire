//
// Created by JS on 24/05/2021.
//

#include "engine/Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace en
{
    Camera::Camera(glm::vec3 pos, glm::vec3 viewDir, glm::vec3 up, float aspectRatio, float fov, float near, float far)
    {
        pos_ = pos;
        viewDir_ = viewDir;
        up_ = up;
        aspectRatio_ = aspectRatio;
        fov_ = fov;
        near_ = near;
        far_ = far;
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

    void Camera::SetPos(glm::vec3 pos)
    {
        pos_ = pos;
    }

    void Camera::SetViewDir(glm::vec3 viewDir)
    {
        viewDir_ = viewDir;
    }

    void Camera::SetUp(glm::vec3 up)
    {
        up_ = up;
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
        pos_ += movement;
    }

    void Camera::RotateViewDir(float phi, float theta)
    {
        // TODO: implement rotation
    }
}
