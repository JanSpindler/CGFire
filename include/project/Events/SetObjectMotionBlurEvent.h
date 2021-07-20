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

    //Sets the motion blur on or off
    class SetObjectMotionBlurEvent : public Event {
    public:
        explicit SetObjectMotionBlurEvent(scene::ObjectManager& objectManager)
                : m_ObjectManager(objectManager){}


        EventType GetTypeID() override {return EventType::SetObjectMotionBlurEvent; };

        Event* Clone() override{
            SetObjectMotionBlurEvent* e = new SetObjectMotionBlurEvent(m_ObjectManager);


            e->m_Obj = m_Obj;
            e->m_MotionBlur = m_MotionBlur;


            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: objectName boolBlurOrNotBlur

            std::string objName = data[0];
            m_Obj = m_ObjectManager.GetRenderObj(objName);

            m_MotionBlur = static_cast<bool>(std::stoi(data[1]));

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_Obj->GetName()
                << m_MotionBlur;

        }

        void OnAction() override {
            m_Obj->blur = m_MotionBlur;
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

            ImGui::Checkbox("Motion Blur ON or OFF", &m_MotionBlur);

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
                m_Description = "Changes Motion blur status of \'"
                        + m_Obj->GetName() + "\' ";

        }

    private:
        scene::ObjectManager& m_ObjectManager;

        RenderObj* m_Obj = nullptr;

        bool m_MotionBlur = false;
    };
}