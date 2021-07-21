//
//Created by vincent on 15.07.2021.
//
#pragma once
#include "project/InitEvents.h"
#include "project/SceneEvent.h"
#include "project/ObjectManager.h"
#include "engine/render/SceneRenderer.hpp"

namespace scene {

    using namespace en;

    //Sets the transform of the camera
    class SetCamTransformEvent : public Event {
    public:
        explicit SetCamTransformEvent(en::Camera& cam)
                : m_Cam(cam){}


        EventType GetTypeID() override {return EventType::SetCamTransformEvent; };

        Event* Clone() override{
            SetCamTransformEvent* e = new SetCamTransformEvent(m_Cam);


            e->m_Position = m_Position;
            e->m_ViewDirection = m_ViewDirection;


            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: 3fPosition 3fViewDirection

            m_Position = glm::vec3(std::stof(data[0]), std::stof(data[1]), std::stof(data[2]));
            m_ViewDirection = glm::vec3(std::stof(data[3]), std::stof(data[4]), std::stof(data[5]));

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_Position.x << m_Position.y << m_Position.z
                    << m_ViewDirection.x << m_ViewDirection.y << m_ViewDirection.z;

        }

        void OnAction() override {
            m_Cam.SetPos(m_Position);
            m_Cam.SetViewDir(m_ViewDirection);
        }


        bool OnImGuiRender() override{
            ImGui::DragFloat3("Position", &m_Position.x, 0.25f);
            ImGui::SameLine();
            if (ImGui::Button("set to current Cam Position")){
                m_Position = m_Cam.GetPos();
            }
            ImGui::DragFloat3("View Direction", &m_ViewDirection.x, 0.25f);
            ImGui::SameLine();
            if (ImGui::Button("set to current Cam view direction")){
                m_ViewDirection = m_Cam.GetViewDir();
            }

            bool optionsOk = true;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj* obj) override{
            return false;
        }

        void UpdateDescription() override{
            m_Description = "Changes Position and view direction of the camera";

        }

    private:
        en::Camera& m_Cam;

        glm::vec3 m_Position;
        glm::vec3 m_ViewDirection;

    };
}