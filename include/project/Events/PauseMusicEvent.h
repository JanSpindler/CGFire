//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
#include <sound/SoundManager.h>
using namespace particle;

namespace scene {

    class PauseMusicEvent : public Event {
    public:
        //Just for the dummy events
        explicit PauseMusicEvent(sound::SoundManager& soundManager)
                : m_SoundManager(soundManager) {
        }


        EventType GetTypeID() override {return EventType::PauseMusicEvent; };

        Event* Clone() override{
            PauseMusicEvent* e = new PauseMusicEvent(m_SoundManager);


            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>:


            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
        }

        void OnAction() override {
            m_SoundManager.PauseMusic();
        }


        bool OnImGuiRender() override{
            bool optionsOk = true;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj*) override{
            return false;
        }

        void UpdateDescription() override{
            m_Description = "Pauses the Music";
        }


    private:
        sound::SoundManager& m_SoundManager;
    };


}