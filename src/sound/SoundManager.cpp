//
//Created by vincent on 19.07.2021.
//

#include <sound/SoundManager.h>
#include <engine/render/Renderable.hpp>
#include <util/StringUtil.h>
#include <framework/imgui_util.hpp>

namespace sound{

        SoundManager::SoundManager(en::Camera& cam)
                : m_Camera(cam){
            util::GetAllFilesByExtension(SOUND_ROOT, ".wav", m_FoundSoundEffectFiles);

            //Load all found sound files
            for (auto& file : m_FoundSoundEffectFiles){
                auto buf = std::make_shared<sf::SoundBuffer>();
                buf->loadFromFile(SOUND_ROOT + file);
                m_MapNamesToBuffers[file] = buf;
                std::string fileAlt = file;
                util::ReplaceAll(fileAlt, "\\", "/");
                m_MapNamesToBuffers[fileAlt] = buf;
                m_SoundBuffers.push_back(buf);
            }

            util::GetAllFilesByExtension(SOUND_ROOT, ".ogg", m_FoundMusicFiles);
        }


        void SoundManager::OnUpdate(){
            auto& pos = m_Camera.GetPos();
            auto& dir = m_Camera.GetViewDir();

            sf::Listener::setPosition(pos.x, pos.y, pos.z);
            sf::Listener::setDirection(dir.x, dir.y, dir.z);

            //update sound positions for sounds that are connected to objects
            for (auto& p : m_CurrentlyPlayingSounds){
                auto pos = p.second->GetPos();
                p.first->setPosition(pos.x, pos.y, pos.z);
            }
        }

        void SoundManager::OnResetTime(){
            m_CurrentlyPlayingSounds.clear();
            m_Music.stop();
        }


        void SoundManager::RegisterSoundAsName(const std::string& SoundName, sf::Sound* sound){

        }

        void SoundManager::RegisterSoundAtObject(sf::Sound* sound, en::RenderObj* obj){
            m_CurrentlyPlayingSounds.emplace_back(sound, obj);
        }

        void SoundManager::PlayMusic(const std::string& file, float volume){
            m_Music.openFromFile(SOUND_ROOT + file);
            if (volume != NAN){
                m_Music.setVolume(volume);
            }
            m_Music.stop();
            m_Music.play();
        }

        void SoundManager::OnImGuiRender(){
            ImGui::Text("Music");
            if (ImGui::DragFloat("Volume", &m_MusicVolume, 1.f, 0.f, 100.f)){
                m_Music.setVolume(m_MusicVolume);
            }

            if (ImGui::SliderFloat("Offset", &m_MusicOffset, 0.f, m_Music.getDuration().asSeconds())){
                m_Music.setPlayingOffset(sf::seconds(m_MusicOffset));
            }
        }
}