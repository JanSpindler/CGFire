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

    //Sets the focus of the camera
    class SetCamFocusEvent : public Event {
    public:
        explicit SetCamFocusEvent(scene::ObjectManager& objectManager,
                                      en::Camera& cam)
                : m_ObjectManager(objectManager),
                m_Cam(cam){}


        EventType GetTypeID() override {return EventType::SetCamFocusEvent; };

        Event* Clone() override{
            SetCamFocusEvent* e = new SetCamFocusEvent(m_ObjectManager, m_Cam);


            e->m_DoFocusObject = m_DoFocusObject;
            e->m_Position = m_Position;
            e->m_Obj = m_Obj;


            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: boolFocusObject 3fPosition if(boolFocusObject){strObjName}else{""}

            m_DoFocusObject = static_cast<bool>(std::stoi(data[0]));
            m_Position = glm::vec3(std::stof(data[1]), std::stof(data[2]), std::stof(data[3]));

            std::string objName = data[4];
            if (!objName.empty())
                m_Obj = m_ObjectManager.GetRenderObj(objName);

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << static_cast<int>(m_DoFocusObject)
                << m_Position.x << m_Position.y << m_Position.z
                    << ((m_Obj != nullptr) ? m_Obj->GetName() : "");

        }

        void OnAction() override {
            if (m_DoFocusObject)
                m_Cam.SetFocus(*m_Obj);
            else
                m_Cam.SetFocus(m_Position);
        }


        bool OnImGuiRender() override{
            if (ImGui::Checkbox("Focus Object", &m_DoFocusObject)){
                if (!m_DoFocusObject){
                    m_Obj = nullptr;
                }
            }

            if (m_DoFocusObject) {
                static std::string s_ObjSelection;

                if (m_Obj != nullptr)
                    s_ObjSelection = m_Obj->GetName();
                else
                    s_ObjSelection = "";

                if (ImGui::BeginCombo("Object", s_ObjSelection.c_str())) {
                    auto &allObjects = m_ObjectManager.GetAllRenderObjects();
                    for (int i = 0; i < allObjects.size(); ++i) {
                        auto &o = allObjects[i];
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
            }
            else{
                ImGui::DragFloat3("Focus Position", &m_Position.x, 1.f);
            }

            bool optionsOk = true;
            if (m_DoFocusObject && m_Obj == nullptr)
                optionsOk = false;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj* obj) override{
            if (obj == m_Obj)
                return true;
            return false;
        }

        void UpdateDescription() override{
            m_Description = "Sets the camera's focus to "
                    + ((m_DoFocusObject && m_Obj != nullptr) ? ("object " + m_Obj->GetName()) :
                       ("position " + util::GetFloatPrecisionFixedString(m_Position.x, 2)
                                  + "/" + util::GetFloatPrecisionFixedString(m_Position.y, 2)
                                  + "/" + util::GetFloatPrecisionFixedString(m_Position.z, 2)));

        }

    private:
        scene::ObjectManager& m_ObjectManager;
        en::Camera& m_Cam;

        bool m_DoFocusObject = false; //if true, focus m_Obj, else focus m_FocusPosition
        glm::vec3 m_Position = glm::vec3(0.f, 0.f, 0.f);
        en::RenderObj* m_Obj = nullptr;
    };
}