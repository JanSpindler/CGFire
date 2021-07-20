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
            m_Flame = Flame::LoadDataFromStrings(data);

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            m_Flame->SaveSpecificDataToCSV(csv);
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