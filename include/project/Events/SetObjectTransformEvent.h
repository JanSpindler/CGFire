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

    //Sets the transform of an object, but you can choose whether to change all three of Position, Rotation and Scale
    class SetObjectTransformEvent : public Event {
    public:
        explicit SetObjectTransformEvent(scene::ObjectManager& objectManager)
                : m_ObjectManager(objectManager){}


        EventType GetTypeID() override {return EventType::SetObjectTransformEvent; };

        Event* Clone() override{
            SetObjectTransformEvent* e = new SetObjectTransformEvent(m_ObjectManager);


            e->m_Obj = m_Obj;
            e->m_ChangePos = m_ChangePos;
            e->m_ChangeRotationAxis = m_ChangeRotationAxis;
            e->m_ChangeRotationAngle = m_ChangeRotationAngle;
            e->m_ChangeScaling = m_ChangeScaling;
            e->m_Position = m_Position;
            e->m_RotationAxis = m_RotationAxis;
            e->m_RotationAngle = m_RotationAngle;
            e->m_Scaling = m_Scaling;


            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: objectName
            // (bool)changePos (bool)changeRotaxis (bool)changeRotAngle (bool)changeScaling
            // 3fPosition 3fRotationAxis 3fRotationAngle 3fScaling


            std::string objName = data[0];
            m_Obj = m_ObjectManager.GetRenderObj(objName);

            m_ChangePos = static_cast<bool>(std::stoi(data[1]));
            m_ChangeRotationAxis = static_cast<bool>(std::stoi(data[2]));
            m_ChangeRotationAngle = static_cast<bool>(std::stoi(data[3]));
            m_ChangeScaling = static_cast<bool>(std::stoi(data[4]));

            m_Position = glm::vec3(std::stof(data[6]), std::stof(data[7]), std::stof(data[8]));
            m_RotationAxis = glm::vec3(std::stof(data[9]), std::stof(data[10]), std::stof(data[11]));
            m_RotationAngle = std::stof(data[12]);
            m_Scaling = glm::vec3(std::stof(data[13]), std::stof(data[14]), std::stof(data[15]));

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_Obj->GetName()
                    << m_ChangePos << m_ChangeRotationAxis << m_ChangeRotationAngle << m_ChangeScaling
                    << m_Position.x << m_Position.y << m_Position.z
                    << m_RotationAxis.x << m_RotationAxis.y << m_RotationAxis.z
                    << m_RotationAngle
                    << m_Scaling.x << m_Scaling.y << m_Scaling.z;

        }

        void OnAction() override {
            if (m_ChangePos)
                m_Obj->Position = m_Position;
            if (m_ChangeRotationAxis)
                m_Obj->RotationAxis = m_RotationAxis;
            if (m_ChangeRotationAngle)
                m_Obj->RotationAngle = m_RotationAngle;
            if (m_ChangeScaling)
                m_Obj->Scaling = m_Scaling;
        }


        bool OnImGuiRender() override{
            static std::string s_ObjSelection;
            static std::string s_SplineSelection;

            if (m_Obj !=  nullptr)
                s_ObjSelection = m_Obj->GetName();
            else
                s_ObjSelection = "";

            if (ImGui::BeginCombo("Object", s_ObjSelection.c_str()))
            {
                auto& allObjs = m_ObjectManager.GetAllRenderObjects();
                for (int i = 0; i < allObjs.size(); ++i)
                {
                    auto& o = allObjs[i];
                    bool is_selected = (s_ObjSelection == o->GetName());
                    ImGui::PushID(i);
                    if (ImGui::Selectable(o->GetName().c_str(), is_selected)) {
                        s_ObjSelection = o->GetName();
                        if (m_Obj != o){
                            m_ChangePos = false;
                            m_ChangeRotationAxis = false;
                            m_ChangeRotationAngle = false;
                            m_ChangeScaling = false;
                        }
                        m_Obj = o;
                        this->UpdateDescription();
                    }
                    ImGui::PopID();
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::Checkbox("Change Position", &m_ChangePos);
            if (m_ChangePos) {
                ImGui::SameLine();
                ImGui::DragFloat3("##change pos", &m_Position.x, 0.005f);
            }
            ImGui::Checkbox("Change Rotation Axis", &m_ChangeRotationAxis);
            if (m_ChangeRotationAxis) {
                ImGui::SameLine();
                ImGui::DragFloat3("##change rot axis", &m_RotationAxis.x, 0.005f);
            }
            ImGui::Checkbox("Change Rotation Angle", &m_ChangeRotationAngle);
            if (m_ChangeRotationAngle) {
                ImGui::SameLine();
                ImGui::DragFloat("##change rot angle", &m_RotationAngle, 0.005f, 0.f, 6.28318530718f);
            }
            ImGui::Checkbox("Change Scaling", &m_ChangeScaling);
            if (m_ChangeScaling) {
                ImGui::SameLine();
                ImGui::DragFloat3("##change Scaling", &m_Scaling.x, 0.005f);
            }

            bool optionsOk = true;
            if ( m_Obj == nullptr)
                optionsOk = false;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj* obj) override{
            if (obj == m_Obj)
                return true;
            return false;
        }

        void UpdateDescription() override{
            if (m_Obj != nullptr)
                m_Description = "Changes Transform of \'"
                        + m_Obj->GetName() + "\' ("
                        + (m_ChangePos ? "Position/" : "")
                        + (m_ChangeRotationAxis ? "RotAxis/" : "")
                        + (m_ChangeRotationAngle ? "RotAngle/" : "")
                        + (m_ChangeScaling ? "Scaling" : "")
                        +")";

        }

    private:
        scene::ObjectManager& m_ObjectManager;

        RenderObj* m_Obj = nullptr;

        bool m_ChangePos = false;
        bool m_ChangeRotationAxis = false;
        bool m_ChangeRotationAngle = false;
        bool m_ChangeScaling = false;

        glm::vec3 m_Position = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 m_RotationAxis = glm::vec3(0.f, 1.f, 0.f);
        float m_RotationAngle = 0.f;
        glm::vec3 m_Scaling = glm::vec3(1.f, 1.f, 1.f);
    };
}