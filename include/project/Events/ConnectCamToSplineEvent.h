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
            e->m_TrackSpeed = m_TrackSpeed;
            e->m_ViewInSplineDir = m_ViewInSplineDir;

            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: splineName trackspeed viewInSplineDir
            std::string splineName = data[0];
            m_Spline = m_ObjectManager.GetSplineByName(splineName).get();
            m_TrackSpeed = std::stof(data[1]);
            m_ViewInSplineDir = static_cast<bool>(std::stoi(data[2]));

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_Spline->GetName() << m_TrackSpeed << static_cast<int>(m_ViewInSplineDir);
        }

        void OnAction() override {
            m_Cam.TrackSpline(m_Spline, m_TrackSpeed, m_ViewInSplineDir);
        }


        bool OnImGuiRender() override{
            static std::string s_SplineSelection;

            if (m_Spline !=  nullptr)
                s_SplineSelection = m_Spline->GetName();
            else
                s_SplineSelection = "";

            if (ImGui::BeginCombo("Spline", s_SplineSelection.c_str()))
            {
                auto& allSplines = m_ObjectManager.GetSplines();
                for (int i = 0; i < allSplines.size(); ++i)
                {
                    auto& s = allSplines[i];
                    bool is_selected = (s_SplineSelection == s->GetName());
                    ImGui::PushID(i);
                    if (ImGui::Selectable(s->GetName().c_str(), is_selected)) {
                        s_SplineSelection = s->GetName();
                        m_Spline = s.get();
                        this->UpdateDescription();
                    }
                    ImGui::PopID();
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::DragFloat("Track Speed", &m_TrackSpeed, 0.01f, 0.f, 999.f);
            ImGui::Checkbox("WatchDir = SplineDir", &m_ViewInSplineDir);

            bool optionsOk = true;
            if (m_Spline == nullptr)
                optionsOk = false;
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
                        + m_Spline->GetName() + "\' ("
                        + (!m_ViewInSplineDir ? "not" : "")
                        + "in spline direction.)";
        }

    private:
        scene::ObjectManager& m_ObjectManager;
        en::Camera& m_Cam;

        Spline3D* m_Spline = nullptr;
        float m_TrackSpeed = 5.f;
        bool m_ViewInSplineDir = true;
    };
}