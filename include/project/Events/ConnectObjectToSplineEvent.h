//
//Created by vincent on 15.07.2021.
//
//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/InitEvents.h"
#include "project/SceneEvent.h"
#include "project/ObjectManager.h"
#include "engine/render/SceneRenderer.hpp"


namespace scene {

    using namespace en;

    //Connects the Cam to a spline
    class ConnectObjectToSplineEvent : public Event {
    public:
        explicit ConnectObjectToSplineEvent(scene::ObjectManager& objectManager,
        )
                : m_ObjectManager(objectManager){}


        EventType GetTypeID() override {return EventType::ConnectObjectToSplineEvent; };

        Event* Clone() override{
            ConnectObjectToSplineEvent* e = new ConnectObjectToSplineEvent(m_ObjectManager);

            e->m_Obj = m_Obj;
            e->m_AnimName = m_AnimName;

            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: objName splineName
            std::string objName = data[0];
            std::string splineName = data[1];

            m_Obj = m_ObjectManager.GetRenderObj(objName);
            m_Spline = m_ObjectManager.GetSplineByName(splineName).get();


            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_Obj->GetName() << m_ObjectManager.GetNameOfSpline(m_Spline);
        }

        void OnAction() override {
            m_ObjectManager.ConnectObjectToSpline(m_Obj, m_Spline);
        }


        bool OnImGuiRender() override{
            static std::string s_ObjSelection;
            static std::string s_SplineSelection;

            if (m_Obj !=  nullptr)
                s_ObjSelection = m_Obj->GetName();
            else
                s_ObjSelection = "";

            if (m_Spline !=  nullptr)
                s_SplineSelection = m_ObjectManager.GetNameOfSpline(m_Spline);
            else
                s_SplineSelection = "";

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
                        m_Obj = o.get();
                        this->UpdateDescription();
                    }
                    ImGui::PopID();
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }


            if (ImGui::BeginCombo("Spline", s_SplineSelection.c_str()))
            {
                auto& allSplines = m_ObjectManager.GetSplines();
                for (int i = 0; i < allSplines.size(); ++i)
                {
                    auto& s = allSplines[i];
                    bool is_selected = (s_SplineSelection == s.second->GetName());
                    ImGui::PushID(i);
                    if (ImGui::Selectable(s.second->GetName().c_str(), is_selected)) {
                        s_SplineSelection = s.second->GetName();
                        m_Spline = s.first.get();
                        this->UpdateDescription();
                    }
                    ImGui::PopID();
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            bool optionsOk = m_Obj != nullptr && m_Spline != nullptr;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj* obj) override{
            if (obj == m_Obj || obj == m_Spline)
                return true;
            return false;
        }

        void UpdateDescription() override{
            if (m_Obj != nullptr)
                m_Description = "Connects \'"
                                + m_Obj->GetName()
                                + "\' to spline "
                                + m_ObjectManager.GetNameOfSpline(m_Spline);
        }

    private:
        scene::ObjectManager& m_ObjectManager;

        RenderObj* m_Obj = nullptr;
        Spline3D* m_Spline = nullptr;
    };
}