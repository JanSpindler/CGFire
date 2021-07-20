//
//Created by vincent on 08.07.2021.
//
#pragma once

#include <particle/Smoke.h>
#include "project/SceneEvent.h"

using namespace particle;

namespace scene {

    class CreateSmokeStreamEvent : public Event {
    public:
        //Just for the dummy events
        explicit CreateSmokeStreamEvent(en::SceneRenderer &sceneRenderer,
                                  SmokeCreator &smokeCreator,
                                  ObjectManager& objectManager)
                : m_SceneRenderer(sceneRenderer),
                  m_SmokeCreator(smokeCreator),
                  m_ObjectManager(objectManager)
                  {
            if (m_DummySmokeStream == nullptr){
                m_DummySmokeStream = std::make_shared<SmokeStream>();
            }
        }


        EventType GetTypeID() override {return EventType::CreateSmokeStreamEvent; };

        Event* Clone() override{
            CreateSmokeStreamEvent* e = new CreateSmokeStreamEvent(m_SceneRenderer, m_SmokeCreator, m_ObjectManager);

            //Copy
            if (m_SmokeStream == nullptr)
                e->m_SmokeStream = std::shared_ptr<SmokeStream>(new SmokeStream(*m_DummySmokeStream.get()));
            else
                e->m_SmokeStream = std::shared_ptr<SmokeStream>(new SmokeStream(*m_SmokeStream.get()));


            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            m_SmokeStream = SmokeStream::LoadDataFromStrings(m_ObjectManager, data);

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            m_SmokeStream->SaveSpecificDataToCSV(csv);
        }

        void OnAction() override {
            m_SmokeCreator.startSmokeStream(m_SmokeStream);
        }

        bool OnImGuiRender() override{
            if (m_SmokeStream == nullptr)
                m_DummySmokeStream->OnImGuiRender();
            else
                m_SmokeStream->OnImGuiRender();


            static std::string s_SplineSelection;


            if (m_SmokeStream == nullptr)
                s_SplineSelection = m_DummySmokeStream->Spline->GetName();
            else
                s_SplineSelection = m_SmokeStream->Spline->GetName();

            if (ImGui::BeginCombo("Spline", s_SplineSelection.c_str()))
            {
                auto& splines = m_ObjectManager.GetSplines();
                for (int i = 0; i < splines.size(); ++i)
                {
                    auto& s = splines[i];
                    bool is_selected = (s_SplineSelection == s->GetName());
                    ImGui::PushID(i);
                    if (ImGui::Selectable(s->GetName().c_str(), is_selected)) {
                        s_SplineSelection = s->GetName();
                        if (m_SmokeStream == nullptr)
                            m_DummySmokeStream->Spline = s;
                        else {
                            m_SmokeStream->Spline = s;
                            this->UpdateDescription();
                        }
                    }
                    ImGui::PopID();
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }


            bool optionsOk = true;

            if (m_SmokeStream == nullptr){ //In Menu Creation of Event
                if (m_DummySmokeStream->Spline == nullptr)
                    optionsOk = false;
            }


            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj* obj) override{
            if (m_SmokeStream->Spline->GetName() == obj->GetName())
                return true;
            return false;
        }

        void UpdateDescription() override{
            m_Description = "Creates the smoke stream \'" + std::string(m_SmokeStream->Name)+ "\'";
        }

    private:
        en::SceneRenderer &m_SceneRenderer;
        SmokeCreator &m_SmokeCreator;
        ObjectManager& m_ObjectManager;
        std::shared_ptr<SmokeStream> m_SmokeStream = nullptr;
        inline static std::shared_ptr<SmokeStream> m_DummySmokeStream = nullptr;
    };
}