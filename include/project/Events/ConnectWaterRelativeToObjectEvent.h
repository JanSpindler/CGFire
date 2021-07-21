//
//Created by vincent on 15.07.2021.
//
#pragma once
#include "project/InitEvents.h"
#include "project/SceneEvent.h"
#include "project/ObjectManager.h"
#include "engine/render/SceneRenderer.hpp"
#include "particle/Water.h"
#include "util/ImGuiDrawing.h"


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
            e->m_RelativeRotation = m_RelativeRotation;

            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: objName WaterName 3fPosition 3fRotationAxis 1fRotationAngle
            std::string objName = data[0];
            std::string waterName = data[1];
            m_RelativePosition = glm::vec3(std::stof(data[2]), std::stof(data[3]),std::stof(data[4]));
            m_RelativeRotation = glm::quat(std::stof(data[5]), std::stof(data[6]), std::stof(data[7]), std::stof(data[8]));

            m_Obj = m_ObjectManager.GetRenderObj(objName);
            strcpy_s(m_WaterName, waterName.c_str());


            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_Obj->GetName() << m_WaterName
                << m_RelativePosition.x << m_RelativePosition.y << m_RelativePosition.z
                << m_RelativeRotation.w << m_RelativeRotation.x << m_RelativeRotation.y << m_RelativeRotation.z;
        }

        void OnAction() override {
            m_WaterCreator.ConnectWaterJetRelativeToObject(
                    m_WaterName, m_Obj, m_RelativePosition, m_RelativeRotation);
        }


        bool OnImGuiRender() override{
            static std::string sObjSelection;

            if (m_Obj !=  nullptr)
                sObjSelection = m_Obj->GetName();
            else
                sObjSelection = "";

            if (ImGui::BeginCombo("Object", sObjSelection.c_str()))
            {
                auto& allObjs = m_ObjectManager.GetAllRenderObjects();
                for (int i = 0; i < allObjs.size(); ++i)
                {
                    auto& o = allObjs[i];
                    bool is_selected = (sObjSelection == o->GetName());
                    if (o != m_Obj) {
                        ImGui::PushID(i);
                        if (ImGui::Selectable(o->GetName().c_str(), is_selected)) {
                            sObjSelection = o->GetName();
                            m_Obj = o;
                            this->UpdateDescription();
                        }
                        ImGui::PopID();
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            if (ImGui::InputText("Water Jet Name", m_WaterName, IM_ARRAYSIZE(m_WaterName))){
                UpdateDescription();
            }


            if (ImGui::DragFloat3("Relative Position", &m_RelativePosition.x, 0.005f)){
                m_WaterCreator.ConnectWaterJetRelativeToObject(m_WaterName, m_Obj, m_RelativePosition, m_RelativeRotation);
            }

            //Draw Rotation Buttons
            if (util::DrawImGuiQuaternionRotationUI(m_RelativeRotation)){
                m_WaterCreator.ConnectWaterJetRelativeToObject(m_WaterName, m_Obj, m_RelativePosition, m_RelativeRotation);
            }


            bool optionsOk = m_Obj != nullptr;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj* obj) override{
            if (obj == m_Obj)
                return true;
            return false;
        }

        void UpdateDescription() override{
            if (m_Obj != nullptr)
                m_Description = "Connects water jet \'"
                                + std::string(m_WaterName)
                                + "\' relative to object "
                                + m_Obj->GetName();
        }

    private:
        scene::ObjectManager& m_ObjectManager;
        particle::WaterCreator& m_WaterCreator;

        RenderObj* m_Obj = nullptr;
        char m_WaterName[32] = "";

        glm::vec3 m_RelativePosition = glm::vec3(0.f, 0.f, 0.f);
        glm::quat m_RelativeRotation = glm::angleAxis(0.f, glm::vec3(0.f, 1.f, 0.f));
    };
}