//
// Created by JS on 24/05/2021.
//

#ifndef CGFIRE_CAMERA_HPP
#define CGFIRE_CAMERA_HPP

#include <glm/glm.hpp>

namespace en
{
    class Camera
    {
    public:
        Camera(glm::vec3 pos, glm::vec3 lookAt, glm::vec3 up, float aspectRatio, float fov);
        ~Camera();

        glm::mat4 GetViewMat() const;
        glm::mat4 GetProjMat() const;

        void SetPos(glm::vec3 pos);
        void SetLookAt(glm::vec3 lookAt);
        void SetUp(glm::vec3 up);
        void SetAspectRatio(float aspectRatio);
        void SetFov(float fov);

        void Move(glm::vec3 movement);
        void RotateLookAt(float phi, float theta);

    private:
        glm::vec3 pos_;
        glm::vec3 lookAt_;
        glm::vec3 up_;
        float aspectRatio_;
        float fov_;
    };
}

#endif //CGFIRE_CAMERA_HPP
