//
//Created by vincent on 15.07.2021.
//
#pragma once
#include "project/InitEvents.h"
#include "project/SceneEvent.h"
#include "project/ObjectManager.h"
#include "engine/render/SceneRenderer.hpp"
#include "util/ImGuiDrawing.h"

namespace scene {

    using namespace en;

    //Slowly rotates the object to the desired Quaternion
    class RotateObjectOverTimeEvent : public Event {
    public:
        explicit RotateObjectOverTimeEvent(scene::ObjectManager& objectManager)
                : m_ObjectManager(objectManager){}


        EventType GetTypeID() override {return EventType::RotateObjectOverTimeEvent; };

        Event* Clone() override{
            RotateObjectOverTimeEvent* e = new RotateObjectOverTimeEvent(m_ObjectManager);


            e->m_Obj = m_Obj;
            e->m_Rotation = m_Rotation;
            e->m_Time = m_Time;


            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: objectName
            // 4fRotation fTime


            std::string objName = data[0];
            m_Obj = m_ObjectManager.GetRenderObj(objName);

            m_Rotation = glm::quat(std::stof(data[1]), std::stof(data[2]), std::stof(data[3]), std::stof(data[4]));
            m_Time = std::stof(data[5]);


            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_Obj->GetName()
                << m_Rotation.w << m_Rotation.x << m_Rotation.y << m_Rotation.z
                    << m_Time;

        }

        void OnAction() override {
            m_ObjectManager.RotateObjectOverTime(m_Obj, m_Rotation, m_Time);
        }


        bool OnImGuiRender() override{
            static std::string s_ObjSelection;

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
                        m_Obj = o;
                        this->UpdateDescription();
                    }
                    ImGui::PopID();
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }


            if (util::DrawImGuiQuaternionRotationUI(m_Rotation)) {
                m_Obj->Quaternion = m_Rotation;
            }

            ImGui::DragFloat("RotationTime", &m_Time, 0.02f, 0.f, 999.f);

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
                m_Description = "Slowly rotates \'"
                        + m_Obj->GetName() + "\'";

        }

    private:
        scene::ObjectManager& m_ObjectManager;

        RenderObj* m_Obj = nullptr;

        glm::quat m_Rotation = glm::toQuat(glm::mat4(1.f));
        float m_Time = 1.f;
    };
}