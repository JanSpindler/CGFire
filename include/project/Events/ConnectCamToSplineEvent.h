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

    //Connects the Cam to a spline
    class ConnectCamToSplineEvent : public Event {
    public:
        explicit ConnectCamToSplineEvent(scene::ObjectManager& objectManager,
                                            en::Camera& cam
        )
                : m_ObjectManager(objectManager),
                m_Cam(cam){}


        EventType GetTypeID() override {return EventType::ConnectCamToSplineEvent; };

        Event* Clone() override{
            ConnectCamToSplineEvent* e = new ConnectCamToSplineEvent(m_ObjectManager, m_Cam);

            e->m_Spline = m_Spline;
            e->m_AnimName = m_AnimName;

            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: splineName
            std::string splineName = data[0];

            m_Spline = m_ObjectManager.GetSplineByName(splineName).get();

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_Obj->GetName() << m_ObjectManager.GetNameOfSpline(m_Spline);
        }

        void OnAction() override {
            //TODO Cam.TrackSpline() ...
        }


        bool OnImGuiRender() override{
            static std::string s_SplineSelection;

            if (m_Spline !=  nullptr)
                s_SplineSelection = m_ObjectManager.GetNameOfSpline(m_Spline);
            else
                s_SplineSelection = "";

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

            bool optionsOk = m_Spline != nullptr;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj* obj) override{
            if (obj == m_Spline)
                return true;
            return false;
        }

        void UpdateDescription() override{
            if (m_Spline != nullptr)
                m_Description = "Connects the camera to spline \'"
                        + m_ObjectManager.GetNameOfSpline(m_Spline) + "\'";
        }

    private:
        scene::ObjectManager& m_ObjectManager;
        en::Camera& m_Cam;

        Spline3D* m_Spline = nullptr;
    };
}