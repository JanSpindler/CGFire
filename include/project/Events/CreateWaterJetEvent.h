//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
using namespace particle;

namespace scene {

    class CreateWaterJetEvent : public Event {
    public:
        //Just for the dummy events
        explicit CreateWaterJetEvent(en::SceneRenderer &sceneRenderer,
                                  WaterCreator &waterCreator)
                : m_SceneRenderer(sceneRenderer),
                  m_WaterCreator(waterCreator) {
            if (m_DummyWaterJet == nullptr){
                m_DummyWaterJet = std::make_shared<WaterJet>();
            }
        }


        EventType GetTypeID() override {return EventType::CreateWaterJetEvent; };

        Event* Clone() override{
            CreateWaterJetEvent* e = new CreateWaterJetEvent(m_SceneRenderer, m_WaterCreator);

            //Copy Flame
            if (m_WaterJet == nullptr) //Then dummy is copied (so a new flame event from menu is created)
                e->m_WaterJet = std::shared_ptr<WaterJet>(new WaterJet(*m_DummyWaterJet.get()));
            else //Then a real event is cloned, so clone it's flame
                e->m_WaterJet = std::shared_ptr<WaterJet>(new WaterJet(*m_WaterJet.get()));


            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            m_WaterJet = WaterJet::LoadDataFromStrings(data);

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            m_WaterJet->SaveSpecificDataToCSV(csv);
        }

        void OnAction() override {
            m_WaterCreator.startWaterJet(m_WaterJet);
        }


        bool OnImGuiRender() override{
            if (m_WaterJet == nullptr)
                m_DummyWaterJet->OnImGuiRender();
            else
                m_WaterJet->OnImGuiRender();

            bool optionsOk = true;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj*) override{
            return false;
        }

        void UpdateDescription() override{
            m_Description = "Creates a the water jet \'" + std::string(m_WaterJet->Name)+ "\'";
        }


    private:
        en::SceneRenderer &m_SceneRenderer;
        WaterCreator &m_WaterCreator;
        std::shared_ptr<WaterJet> m_WaterJet = nullptr;
        inline static std::shared_ptr<WaterJet> m_DummyWaterJet = nullptr;
    };


}