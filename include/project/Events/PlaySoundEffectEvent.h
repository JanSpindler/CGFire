//
//Created by vincent on 19.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
#include <project/ObjectManager.h>
#include <sound/SoundManager.h>

using namespace particle;

namespace scene {

    class PlaySoundEffectEvent : public Event {
    public:
        //Just for the dummy events
        explicit PlaySoundEffectEvent(sound::SoundManager& soundManager,
                scene::ObjectManager& objectManager)
         : m_SoundManager(soundManager),
         m_ObjectManager(objectManager){

        }

        EventType GetTypeID() override {return EventType::PlaySoundEffectEvent; };

        Event* Clone() override{
            PlaySoundEffectEvent* e = new PlaySoundEffectEvent(m_SoundManager, m_ObjectManager);

            strcpy_s(e->m_SoundName, m_SoundName);
            e->m_SoundFile = m_SoundFile;
            e->m_Sound = m_Sound;
            e->m_Volume = m_Volume;
            e->m_Attenuation = m_Attenuation;
            e->m_MinDistance = m_MinDistance;
            e->m_Loop = m_Loop;
            e->m_Obj = m_Obj;
            e->m_Position = m_Position;
            e->m_RelativeToObj = m_RelativeToObj;

            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: strSoundName strSoundFile bLoop fVolume fAttenuation fMinDistance
            // 3fPos boolRelativeToObj strObjName

            strcpy_s(m_SoundName, data[0].c_str());
            m_SoundFile = data[1];
            m_Loop = static_cast<bool>(std::stoi(data[2]));
            m_Volume = std::stof(data[3]);
            m_Attenuation = std::stof(data[4]);
            m_MinDistance = std::stof(data[5]);
            m_Position = glm::vec3(std::stof(data[6]), std::stof(data[7]), std::stof(data[8]));
            m_RelativeToObj = static_cast<bool>(std::stoi(data[9]));

            if (m_RelativeToObj) {
                std::string objName = data[10];

                m_Obj = m_ObjectManager.GetRenderObj(objName);
            }

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << std::string (m_SoundName)
                << m_SoundFile
                << static_cast<int>(m_Loop)
                << m_Volume
                << m_Attenuation
                << m_MinDistance
                << m_Position.x << m_Position.y << m_Position.z
                << static_cast<int>(m_RelativeToObj) << (m_Obj == nullptr ? "" : m_Obj->GetName());
        }

        void OnResetTime(){
            Event::OnResetTime();
            m_Sound.stop();
        }
        void OnAction() override {
            m_Sound.setBuffer(*m_SoundManager.GetSoundBufferFromFile(m_SoundFile));
            m_Sound.setLoop(m_Loop);
            m_Sound.setVolume(m_Volume);
            m_Sound.setAttenuation(m_Attenuation);
            m_Sound.setMinDistance(m_MinDistance);

            m_SoundManager.RegisterSoundAsName(m_SoundName, &m_Sound);
            if (m_RelativeToObj){
                if (m_Obj != nullptr) {
                    m_SoundManager.RegisterSoundAtObject(&m_Sound, m_Obj);
                }
            }
            else{
                m_Sound.setPosition(m_Position.x, m_Position.y, m_Position.z);
            }
            m_Sound.play();
        }


        bool OnImGuiRender() override{
            if (ImGui::InputText("Sound Name", m_SoundName, IM_ARRAYSIZE(m_SoundName))){
                UpdateDescription();
            }

            static std::string s_SoundSelection;

            if (!m_SoundFile.empty())
                s_SoundSelection = m_SoundFile;
            else
                s_SoundSelection = "";

            if (ImGui::BeginCombo("Sound ", s_SoundSelection.c_str()))
            {
                auto& allFiles = m_SoundManager.GetFoundSoundEffectFiles();
                for (int i = 0; i < allFiles.size(); ++i)
                {
                    auto& f = allFiles[i];
                    bool is_selected = (s_SoundSelection == f);
                    ImGui::PushID(i);
                    if (ImGui::Selectable(f.c_str(), is_selected)) {
                        s_SoundSelection = f;
                        m_SoundFile = s_SoundSelection;
                        m_Sound.setBuffer(*m_SoundManager.GetSoundBufferFromFile(m_SoundFile));
                        this->UpdateDescription();
                    }
                    ImGui::PopID();
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            ImGui::Checkbox("Loop ", &m_Loop);
            if (ImGui::DragFloat("Volume", &m_Volume, 1.f, 0.f, 100.f)){
                m_Sound.setVolume(m_Volume);
            }
            ImGui::Checkbox("Relative to Object", &m_RelativeToObj);

            if (m_RelativeToObj) {
                static std::string s_ObjSelection;

                if (m_Obj != nullptr)
                    s_ObjSelection = m_Obj->GetName();
                else
                    s_ObjSelection = "";

                if (ImGui::BeginCombo("Object", s_ObjSelection.c_str())) {
                    auto &allObjects = m_ObjectManager.GetAllRenderObjects();
                    for (int i = 0; i < allObjects.size(); ++i) {
                        auto &o = allObjects[i];
                        bool is_selected = (s_ObjSelection == o->GetName());
                        ImGui::PushID(i);
                        if (ImGui::Selectable(o->GetName().c_str(), is_selected)) {
                            s_ObjSelection = o->GetName();
                            m_Obj = o;
                            this->UpdateDescription();
                        }
                        ImGui::PopID();
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }
            else{
                if (ImGui::DragFloat3("Position", &m_Position.x, 1.f)){
                    m_Sound.setPosition(m_Position.x, m_Position.y, m_Position.z);
                }
            }

            if (ImGui::DragFloat("Attenuation", &m_Attenuation, 0.1f, 0.f, 30.f)){
                m_Sound.setAttenuation(m_Attenuation);
            }
            if (ImGui::DragFloat("MinDistance", &m_MinDistance, 0.1f, 0.f, 99999.f)){
                m_Sound.setMinDistance(m_MinDistance);
            }

            if (m_Sound.getBuffer() != nullptr) {
                m_SoundTime = m_Sound.getPlayingOffset().asSeconds();
                if (ImGui::SliderFloat("Sound Time", &m_SoundTime, 0.f,
                                       m_Sound.getBuffer()->getDuration().asSeconds())) {
                    m_Sound.setPlayingOffset(sf::seconds(m_SoundTime));
                }
            }

            bool optionsOk = true;
            if (std::string(m_SoundName).empty() || m_SoundFile.empty())
                optionsOk = false;
            if (m_RelativeToObj && m_Obj == nullptr)
                optionsOk = false;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj* obj) override{
            if (obj == m_Obj)
                return true;
            return false;
        }

        void UpdateDescription() override{
            m_Description = "Plays the sound \'" + std::string(m_SoundName) + "\'"
                    + (m_Loop ? "(looped)" : "")
                    + (m_RelativeToObj ?
                    (m_Obj != nullptr ? ("relative to object " + m_Obj->GetName()) : "") : "");
        }



    private:
        sound::SoundManager& m_SoundManager;
        scene::ObjectManager& m_ObjectManager;

        char m_SoundName[32] = "";
        std::string m_SoundFile;

        bool m_Loop = false;
        float m_Volume = 50.f;
        glm::vec3 m_Position = glm::vec3(0.f,0.f, 0.f); //only used if not relative to obj
        bool m_RelativeToObj = false;

        float m_Attenuation = 0.5f;
        float m_MinDistance = 5.f;

        en::RenderObj* m_Obj = nullptr;
        sf::Sound m_Sound;


        // current sound time, saved here for ImGui
        float m_SoundTime = 0.f;
    };


}