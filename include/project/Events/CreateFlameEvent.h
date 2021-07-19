//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
using namespace particle;

namespace scene {

    class CreateFlameEvent : public Event {
    public:
        //Just for the dummy events
        explicit CreateFlameEvent(en::SceneRenderer &sceneRenderer,
                                  FireCreator &fireCreator)
                : m_SceneRenderer(sceneRenderer),
                  m_FireCreator(fireCreator) {
            if (m_DummyFlame == nullptr){
                m_DummyFlame = std::make_shared<Flame>();
            }
        }


        EventType GetTypeID() override {return EventType::CreateFlameEvent; };

        Event* Clone() override{
            CreateFlameEvent* e = new CreateFlameEvent(m_SceneRenderer, m_FireCreator);

            //Copy Flame
            if (m_Flame == nullptr) //Then dummy is copied (so a new flame event from menu is created)
                e->m_Flame = std::shared_ptr<Flame>(new Flame(*m_DummyFlame.get()));
            else //Then a real event is cloned, so clone it's flame
                e->m_Flame = std::shared_ptr<Flame>(new Flame(*m_Flame.get()));


            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: see "Relevant Data" in Fire.h

            std::string Name = data[0];
            glm::vec3 Position = glm::vec3(std::stof(data[1]), std::stof(data[2]), std::stof(data[3]));
            glm::vec3 PositionVariation = glm::vec3(std::stof(data[4]), std::stof(data[5]), std::stof(data[6]));
            int ParticlesPerEmit = std::stoi(data[7]);
            float BuildUpTime = std::stof(data[8]);
            float ExpireTime = std::stof(data[9]);
            float ParticleLifeTime = std::stof(data[10]);
            float ParticleLifeTimeVariation = std::stof(data[11]);


            m_Flame = std::make_shared<Flame>(Name.c_str(), Position, PositionVariation, ParticlesPerEmit,
                                              BuildUpTime, ExpireTime, ParticleLifeTime, ParticleLifeTimeVariation);

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_Flame->Name
                << m_Flame->Position.x << m_Flame->Position.y << m_Flame->Position.z
                << m_Flame->PositionVariation.x << m_Flame->PositionVariation.y << m_Flame->PositionVariation.z
                << m_Flame->ParticlesPerEmit
                << m_Flame->BuildUpTime
                << m_Flame->ExpiringTime
                << m_Flame->ParticleLifeTime
                << m_Flame->ParticleLifeTimeVariation;
        }

        void OnAction() override {
            m_FireCreator.startFlame(m_Flame);
            //TODO Point Light
            //m_SceneRenderer.AddPointLight(m_Flame.get());
        }


        bool OnImGuiRender() override{
            if (m_Flame == nullptr)
                m_DummyFlame->OnImGuiRender();
            else
                m_Flame->OnImGuiRender();

            bool optionsOk = true;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj*) override{
            return false;
        }

        void UpdateDescription() override{
            m_Description = "Creates a the flame \'" + std::string(m_Flame->Name)+ "\'";
        }


    private:
        en::SceneRenderer &m_SceneRenderer;
        FireCreator &m_FireCreator;
        std::shared_ptr<Flame> m_Flame = nullptr;
        inline static std::shared_ptr<Flame> m_DummyFlame = nullptr;
    };


}