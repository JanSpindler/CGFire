//
//Created by vincent on 15.07.2021.
//
#pragma once
#include "project/InitEvents.h"
#include "project/SceneEvent.h"
#include "project/ObjectManager.h"
#include "engine/render/SceneRenderer.hpp"
#include "particle/Water.h"


namespace scene {

    using namespace en;

    //Connects a water jet to an object with relative position and rotation
    class ConnectWaterRelativeToObjectEvent : public Event {
    public:
        explicit ConnectWaterRelativeToObjectEvent(scene::ObjectManager& objectManager,
                                                   particle::WaterCreator& waterCreator)
                : m_ObjectManager(objectManager),
                m_WaterCreator(waterCreator){}


        EventType GetTypeID() override {return EventType::ConnectWaterRelativeToObjectEvent; };

        Event* Clone() override{
            ConnectWaterRelativeToObjectEvent* e = new ConnectWaterRelativeToObjectEvent(m_ObjectManager, m_WaterCreator);

            e->m_Obj = m_Obj;
            strcpy_s(e->m_WaterName, m_WaterName);
            e->m_RelativePosition = m_RelativePosition;
            e->m_RelativeRotationAxis = m_RelativeRotationAxis;
            e->m_RelativeRotationAngle = m_RelativeRotationAngle;

            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: objName WaterName 3fPosition 3fRotationAxis 1fRotationAngle
            std::string objName = data[0];
            std::string waterName = data[1];
            m_RelativePosition = glm::vec3(std::stof(data[2]), std::stof(data[3]),std::stof(data[4]));
            m_RelativeRotationAxis = glm::vec3(std::stof(data[5]), std::stof(data[6]), std::stof(data[7]));
            m_RelativeRotationAngle = std::stof(data[8]);

            m_Obj = m_ObjectManager.GetRenderObj(objName);
            strcpy_s(m_WaterName, waterName.c_str());


            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_Obj->GetName() << m_WaterName
                << m_RelativePosition.x << m_RelativePosition.y << m_RelativePosition.z
                << m_RelativeRotationAxis.x << m_RelativeRotationAxis.y << m_RelativeRotationAxis.z << m_RelativeRotationAngle;
        }

        void OnAction() override {
            m_WaterCreator.ConnectWaterJetRelativeToObject(m_WaterName, m_obj);
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
            ImGui::DragFloat("Relative Rotation Angle", &m_RelativeRotationAngle, 1., 0.f, 6.28318530718f);
            ImGui::DragFloat3("Relative Rotation Axis", &m_RelativeRotationAxis.x, 0.005f);


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
        particle::WaterCreator& m_WaterCreator;

        RenderObj* m_Obj = nullptr;
        char m_WaterName[32] = "";

        glm::vec3 m_RelativePosition = glm::vec3(0.f, 0.f, 0.f);
        glm::vec3 m_RelativeRotationAxis = glm::vec3(0.f, 0.f, 0.f);
        float m_RelativeRotationAngle = 0.f;
    };
}