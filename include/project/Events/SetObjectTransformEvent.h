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
            e->m_ChangeRotation = m_ChangeRotation;
            e->m_ChangeScaling = m_ChangeScaling;
            e->m_Position = m_Position;
            e->m_EulerAngles = m_EulerAngles;
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
            m_ChangeRotation = static_cast<bool>(std::stoi(data[2]));
            m_ChangeScaling = static_cast<bool>(std::stoi(data[3]));

            m_Position = glm::vec3(std::stof(data[4]), std::stof(data[5]), std::stof(data[6]));
            m_EulerAngles = glm::vec3(std::stof(data[7]), std::stof(data[8]), std::stof(data[9]));
            m_Scaling = glm::vec3(std::stof(data[10]), std::stof(data[11]), std::stof(data[12]));

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_Obj->GetName()
                << m_ChangePos << m_ChangeRotation << m_ChangeScaling
                << m_Position.x << m_Position.y << m_Position.z
                << m_EulerAngles.x << m_EulerAngles.y << m_EulerAngles.z
                    << m_Scaling.x << m_Scaling.y << m_Scaling.z;

        }

        void OnAction() override {
            if (m_ChangePos)
                m_Obj->Position = m_Position;
            if (m_ChangeRotation)
                m_Obj->EulerAngles = m_EulerAngles;
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
                            m_ChangeRotation = false;
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
            ImGui::Checkbox("Change Rotation", &m_ChangeRotation);
            if (m_ChangeRotation) {
                ImGui::SameLine();
                ImGui::DragFloat3("##change euler angles", &m_EulerAngles.x, 0.005f, 0.f, 6.28318530718f);
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
                        + (m_ChangeRotation ? "EulerAngles/" : "")
                        + (m_ChangeScaling ? "Scaling" : "")
                        +")";

        }

    private:
        scene::ObjectManager& m_ObjectManager;

        RenderObj* m_Obj = nullptr;

        bool m_ChangePos = false;
        bool m_ChangeRotation = false;
        bool m_ChangeScaling = false;

        glm::vec3 m_Position = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 m_EulerAngles = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 m_Scaling = glm::vec3(1.f, 1.f, 1.f);
    };
}