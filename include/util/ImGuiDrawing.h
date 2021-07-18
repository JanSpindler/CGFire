//
//Created by vincent on 18.07.2021.
//

#ifndef CGFIRE_IMGUIDRAWING_H
#define CGFIRE_IMGUIDRAWING_H
#pragma once

#include "framework/imgui_util.hpp"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>

namespace util{

    inline void DrawImGuiQuaternionRotationUI(glm::quat& quaternion){
        const float speed = 5.f;
        float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
        ImGui::PushButtonRepeat(true);
        if (ImGui::ArrowButton("##rotxpos", ImGuiDir_Left)) {
            quaternion = glm::rotate(quaternion, glm::radians(speed),
                                     glm::vec3(1.f, 0.f, 0.f));
        }
        ImGui::SameLine(0.0f, spacing);
        if (ImGui::ArrowButton("##rotxneg", ImGuiDir_Right)) {
            quaternion = glm::rotate(quaternion, glm::radians(-speed),
                                     glm::vec3(1.f, 0.f, 0.f));
        }
        ImGui::SameLine();
        ImGui::Text("Rotate over x-Axis");


        if (ImGui::ArrowButton("##rotypos", ImGuiDir_Left)) {
            quaternion = glm::rotate(quaternion, glm::radians(speed),
                                     glm::vec3(0.f, 1.f, 0.f));
        }
        ImGui::SameLine(0.0f, spacing);
        if (ImGui::ArrowButton("##rotyneg", ImGuiDir_Right)) {
            quaternion = glm::rotate(quaternion, glm::radians(-speed),
                                     glm::vec3(0.f, 1.f, 0.f));
        }
        ImGui::SameLine();
        ImGui::Text("Rotate over y-Axis");
        ImGui::SameLine();
        if (ImGui::Button("Reset Rotation")){
            quaternion = glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.f));
        }

        if (ImGui::ArrowButton("##rotzpos", ImGuiDir_Left)) {
            quaternion = glm::rotate(quaternion, glm::radians(speed),
                                     glm::vec3(0.f, 0.f, 1.f));
        }
        ImGui::SameLine(0.0f, spacing);
        if (ImGui::ArrowButton("##rotzneg", ImGuiDir_Right)) {
            quaternion = glm::rotate(quaternion, glm::radians(-speed),
                                     glm::vec3(0.f, 0.f, 1.f));
        }
        ImGui::SameLine();
        ImGui::Text("Rotate over z-Axis");
        ImGui::PopButtonRepeat();
    }

}
#endif //CGFIRE_IMGUIDRAWING_H
