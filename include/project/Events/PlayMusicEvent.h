//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
#include <sound/SoundManager.h>
using namespace particle;

namespace scene {

    class PlayMusicEvent : public Event {
    public:
        //Just for the dummy events
        explicit PlayMusicEvent(sound::SoundManager& soundManager)
                : m_SoundManager(soundManager) {
        }


        EventType GetTypeID() override {return EventType::PlayMusicEvent; };

        Event* Clone() override{
            PlayMusicEvent* e = new PlayMusicEvent(m_SoundManager);

            e->m_FileName  = m_FileName;
            e->m_Volume = m_Volume;

            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: name volume

            m_FileName = data[0];
            m_Volume = std::stof(data[1]);

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_FileName << m_Volume;
        }

        void OnAction() override {
            m_SoundManager.PlayMusic(m_FileName, m_Volume);
        }


        bool OnImGuiRender() override{
            static std::string s_MusicSelection;

            if (!m_FileName.empty())
                s_MusicSelection = m_FileName;
            else
                s_MusicSelection = "";

            if (ImGui::BeginCombo("Music ", s_MusicSelection.c_str()))
            {
                auto& allFiles = m_SoundManager.GetFoundMusicFiles();
                for (int i = 0; i < allFiles.size(); ++i)
                {
                    auto& f = allFiles[i];
                    bool is_selected = (s_MusicSelection == f);
                    ImGui::PushID(i);
                    if (ImGui::Selectable(f.c_str(), is_selected)) {
                        s_MusicSelection = f;
                        m_FileName = s_MusicSelection;
                        this->UpdateDescription();
                    }
                    ImGui::PopID();
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::DragFloat("Volume", &m_Volume, 1.f, 0.f, 100.f);

            bool optionsOk = true;
            if (m_FileName.empty())
                optionsOk = false;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj*) override{
            return false;
        }

        void UpdateDescription() override{
            m_Description = "Plays Music \'" + m_FileName + "\'";
        }


    private:
        sound::SoundManager& m_SoundManager;

        std::string m_FileName;
        float m_Volume = 50.f;
    };


}