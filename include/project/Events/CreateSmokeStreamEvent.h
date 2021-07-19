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
            //<specific data>: see "Relevant Data" in Smoke.h, except Spline will be represented by its name

            std::string Name = data[0];
            std::string SplineName = data[1];
            glm::vec3 PositionVariation = glm::vec3(std::stof(data[2]), std::stof(data[3]), std::stof(data[4]));
            int ParticlesPerSecond = std::stoi(data[5]);
            float Speed = std::stof(data[6]);
            float SpeedVariation = std::stof(data[7]);
            float SizeBegin = std::stof(data[8]);
            float SizeEnd = std::stof(data[9]);
            float SizeVariationFactor = std::stof(data[10]);
            float BuildUpTime = std::stof(data[11]);
            float ExpiringTime = std::stof(data[12]);

            m_SmokeStream = std::make_shared<SmokeStream>(Name.c_str(), m_ObjectManager.GetSplineByName(SplineName),
                                                          PositionVariation, ParticlesPerSecond,
                                                          Speed, SpeedVariation, SizeBegin, SizeEnd, SizeVariationFactor,
                                                          BuildUpTime, ExpiringTime);

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << std::string(m_SmokeStream->Name)
                << m_ObjectManager.GetNameOfSpline(m_SmokeStream->Spline.get())
                << m_SmokeStream->PositionVariation.x << m_SmokeStream->PositionVariation.y << m_SmokeStream->PositionVariation.z
                << m_SmokeStream->ParticlesPerSecond
                << m_SmokeStream->Speed
                << m_SmokeStream->SpeedVariation
                << m_SmokeStream->SizeBegin
                << m_SmokeStream->SizeEnd
                << m_SmokeStream->SizeVariationFactor
                << m_SmokeStream->BuildUpTime
                << m_SmokeStream->ExpiringTime;
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
                s_SplineSelection = m_ObjectManager.GetNameOfSpline(m_DummySmokeStream->Spline.get());
            else
                s_SplineSelection = m_ObjectManager.GetNameOfSpline(m_SmokeStream->Spline.get());

            if (ImGui::BeginCombo("Spline", s_SplineSelection.c_str()))
            {
                auto& splines = m_ObjectManager.GetSplines();
                for (int i = 0; i < splines.size(); ++i)
                {
                    auto& s = splines[i];
                    bool is_selected = (s_SplineSelection == s.second->GetName());
                    ImGui::PushID(i);
                    if (ImGui::Selectable(s.second->GetName().c_str(), is_selected)) {
                        s_SplineSelection = s.second->GetName();
                        if (m_SmokeStream == nullptr)
                            m_DummySmokeStream->Spline = s.first;
                        else {
                            m_SmokeStream->Spline = s.first;
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
            if (m_ObjectManager.GetNameOfSpline(m_SmokeStream->Spline.get()) == obj->GetName())
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