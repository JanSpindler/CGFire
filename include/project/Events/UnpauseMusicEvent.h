//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
#include <sound/SoundManager.h>
using namespace particle;

namespace scene {

    class UnpauseMusicEvent : public Event {
    public:
        //Just for the dummy events
        explicit UnpauseMusicEvent(sound::SoundManager& soundManager)
                : m_SoundManager(soundManager) {
        }


        EventType GetTypeID() override {return EventType::UnpauseMusicEvent; };

        Event* Clone() override{
            UnpauseMusicEvent* e = new UnpauseMusicEvent(m_SoundManager);


            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>:


            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
        }

        void OnAction() override {
            m_SoundManager.UnpauseMusic();
        }


        bool OnImGuiRender() override{
            bool optionsOk = true;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj*) override{
            return false;
        }

        void UpdateDescription() override{
            m_Description = "Unpauses the Music";
        }


    private:
        sound::SoundManager& m_SoundManager;
    };


}