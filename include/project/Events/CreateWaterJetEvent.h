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
            //<specific data>: see "Relevant Data" in Water.h

            std::string Name = data[0];
            glm::vec3 Position = glm::vec3(std::stof(data[1]), std::stof(data[2]), std::stof(data[3]));
            glm::vec3 PositionVariation = glm::vec3(std::stof(data[4]), std::stof(data[5]), std::stof(data[6]));
            glm::vec3 WaterDirection = glm::vec3(std::stof(data[7]), std::stof(data[8]), std::stof(data[9]));
            float Speed = std::stof(data[10]);
            float SpeedVariationFactor = std::stof(data[11]);
            int ParticlesPerSecond = std::stoi(data[12]);
            float BuildUpTime = std::stof(data[13]);
            float ExpireTime = std::stof(data[14]);
            float ParticleLifeTime = std::stof(data[15]);
            float ParticleLifeTimeVariation = std::stof(data[16]);


            m_WaterJet = std::make_shared<WaterJet>(Name.c_str(), Position, PositionVariation, WaterDirection, Speed, SpeedVariationFactor,
                            ParticlesPerSecond, BuildUpTime, ExpireTime, ParticleLifeTime, ParticleLifeTimeVariation);

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_WaterJet->Name
                << m_WaterJet->Position.x << m_WaterJet->Position.y << m_WaterJet->Position.z
                << m_WaterJet->PositionVariation.x << m_WaterJet->PositionVariation.y << m_WaterJet->PositionVariation.z
                << m_WaterJet->WaterDirection.x << m_WaterJet->WaterDirection.y << m_WaterJet->WaterDirection.z
                << m_WaterJet->Speed
                << m_WaterJet->SpeedVariationFactor
                << m_WaterJet->ParticlesPerSecond
                << m_WaterJet->BuildUpTime
                << m_WaterJet->ExpiringTime
                << m_WaterJet->ParticleLifeTime
                << m_WaterJet->ParticleLifeTimeVariation;
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