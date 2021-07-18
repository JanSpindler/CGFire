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

    //Connects an object to another object with relative position and rotation
    class ConnectObjectRelativeToObjectEvent : public Event {
    public:
        explicit ConnectObjectRelativeToObjectEvent(scene::ObjectManager& objectManager)
                : m_ObjectManager(objectManager){}


        EventType GetTypeID() override {return EventType::ConnectObjectRelativeToObjectEvent; };

        Event* Clone() override{
            ConnectObjectRelativeToObjectEvent* e = new ConnectObjectRelativeToObjectEvent(m_ObjectManager);

            e->m_ObjCarried = m_ObjCarried;
            e->m_ObjCarry = m_ObjCarry;
            e->m_RelativePosition = m_RelativePosition;
            e->m_RelativeEulerAngles = m_RelativeEulerAngles;

            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: objCarryName objCarriedName 3fPosition 3fEulerAngles
            std::string objCarryName = data[0];
            std::string objCarriedName = data[1];
            m_RelativePosition = glm::vec3(std::stof(data[2]), std::stof(data[3]),std::stof(data[4]));
            m_RelativeEulerAngles = glm::vec3(std::stof(data[5]), std::stof(data[6]), std::stof(data[7]));

            m_ObjCarried = m_ObjectManager.GetRenderObj(objCarriedName);
            m_ObjCarry = m_ObjectManager.GetRenderObj(objCarryName);


            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_ObjCarried->GetName() << m_ObjCarry->GetName()
                << m_RelativePosition.x << m_RelativePosition.y << m_RelativePosition.z
                << m_RelativeEulerAngles.x << m_RelativeEulerAngles.y << m_RelativeEulerAngles.z;
        }

        void OnAction() override {
            m_ObjectManager.ConnectObjectRelativeToObject(m_ObjCarried, m_ObjCarry, m_RelativePosition,
                                                          m_RelativeEulerAngles);
        }


        bool OnImGuiRender() override{
            static std::string s_ObjCarriedSelection;
            static std::string s_ObjCarrySelection;


            if (m_ObjCarried !=  nullptr)
                s_ObjCarriedSelection = m_ObjCarried->GetName();
            else
                s_ObjCarriedSelection = "";

            if (m_ObjCarry !=  nullptr)
                s_ObjCarrySelection = m_ObjCarry->GetName();
            else
                s_ObjCarrySelection = "";


            if (ImGui::BeginCombo("Carried Object", s_ObjCarriedSelection.c_str()))
            {
                auto& allObjs = m_ObjectManager.GetAllRenderObjects();
                for (int i = 0; i < allObjs.size(); ++i)
                {
                    auto& o = allObjs[i];
                    bool is_selected = (s_ObjCarriedSelection == o->GetName());
                    if (o != m_ObjCarry) {
                        ImGui::PushID(i);
                        if (ImGui::Selectable(o->GetName().c_str(), is_selected)) {
                            s_ObjCarriedSelection = o->GetName();
                            m_ObjCarried = o;
                            this->UpdateDescription();
                        }
                        ImGui::PopID();
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }


            if (ImGui::BeginCombo("Carry Object", s_ObjCarrySelection.c_str()))
            {
                auto& allObjs = m_ObjectManager.GetAllRenderObjects();
                for (int i = 0; i < allObjs.size(); ++i)
                {
                    auto& o = allObjs[i];
                    bool is_selected = (s_ObjCarrySelection == o->GetName());
                    if (o != m_ObjCarried) {
                        ImGui::PushID(i);
                        if (ImGui::Selectable(o->GetName().c_str(), is_selected)) {
                            s_ObjCarrySelection = o->GetName();
                            m_ObjCarry = o;
                            this->UpdateDescription();
                        }
                        ImGui::PopID();
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }



            ImGui::DragFloat3("Relative Position", &m_RelativePosition.x, 0.005f);
            ImGui::DragFloat3("Relative Euler Angles", &m_RelativeEulerAngles.x, 0.005f, 0.f, 6.28318530718f);


            bool optionsOk = m_ObjCarried != nullptr && m_ObjCarry != nullptr;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj* obj) override{
            if (obj == m_ObjCarried || obj == m_ObjCarry)
                return true;
            return false;
        }

        void UpdateDescription() override{
            if (m_ObjCarried != nullptr && m_ObjCarry != nullptr)
                m_Description = "Connects \'"
                                + m_ObjCarried->GetName()
                                + "\' relative to object "
                                + m_ObjCarry->GetName();
        }

    private:
        scene::ObjectManager& m_ObjectManager;

        RenderObj* m_ObjCarried = nullptr;
        RenderObj* m_ObjCarry = nullptr;

        glm::vec3 m_RelativePosition = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 m_RelativeEulerAngles = glm::vec3(0.f, 0.f, 0.f);
    };
}