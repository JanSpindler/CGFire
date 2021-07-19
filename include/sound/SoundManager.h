//
//Created by vincent on 19.07.2021.
//

#pragma once

#include <SFML/Audio.hpp>
#include <engine/config.hpp>
#include <util/FileFinder.h>
#include <engine/Camera.hpp>
#include <map>

namespace en{
    class RenderObj;
}
namespace sound{

    class SoundManager{
    public:
        SoundManager(en::Camera& cam);

        void OnUpdate();
        void OnResetTime();


        // So we can maybe later stop sounds by name
        void RegisterSoundAsName(const std::string& SoundName, sf::Sound* sound);

        void RegisterSoundAtObject(sf::Sound* sound, en::RenderObj* obj);

        const std::vector<std::string>& GetFoundSoundEffectFiles(){ return m_FoundSoundEffectFiles; }

        const std::vector<std::string>& GetFoundMusicFiles(){ return m_FoundMusicFiles; }

        std::shared_ptr<sf::SoundBuffer> GetSoundBufferFromFile(const std::string& file){
            return m_MapNamesToBuffers[file];
        }

        void PlayMusic(const std::string& file, float volume = NAN);

        void OnImGuiRender();
    private:
        en::Camera& m_Camera;

        std::vector<std::string> m_FoundSoundEffectFiles;
        std::vector<std::shared_ptr<sf::SoundBuffer>> m_SoundBuffers;
        std::map<std::string, std::shared_ptr<sf::SoundBuffer>> m_MapNamesToBuffers;

        std::list<std::pair<sf::Sound*, en::RenderObj*>> m_CurrentlyPlayingSounds;

        std::vector<std::string> m_FoundMusicFiles;
        sf::Music m_Music;
        float m_MusicVolume = 50.f;
        float m_MusicOffset = 0.f;
    };
}