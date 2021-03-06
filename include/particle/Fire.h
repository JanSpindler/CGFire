//
//Created by vincent on 28.05.2021.
// Creation of Flames
#pragma once

#include "engine/render/GLTexture.hpp"
#include "particle/ParticleSystem.h"
#include <framework/imgui_util.hpp>
#include "engine/render/Light.hpp"
#include <sound/SoundManager.h>
#include <util/CSVWriter.h>

namespace particle{

    /*A flame of fire can be placed in the scene with individual properties.*/
    class Flame : public en::PointLight{
        friend class FireCreator;
    public:
        Flame(const char name[32] = "FlameNoName",
              const glm::vec3& position = glm::vec3(0.f, 5.f, 0.f),
              const glm::vec3& positionVariation = glm::vec3(1.f, 0.f, 1.f),
              int particlesPerEmit = 25,
              float buildUpTime = 5.f,
              float expiringTime = 5.f,
              float particleLifeTime = 1.f,
              float particleLifeTimeVariation = 0.2f,
              const glm::vec3& velocity = { 0.0f, 8.0f, 0.0f },
              const glm::vec3& velocityVariation = { 4.5f, 10.0f, 4.5f },
              float sizeBegin = 1.f,
              float sizeEnd = 0.8f);


        //Relevant Data
        char Name[32];
        glm::vec3 Position;
        glm::vec3 PositionVariation;
        int ParticlesPerEmit;
        float BuildUpTime; //the amount of time the fire takes to come to its peak
        float ExpiringTime; //the amount of time the fire takes to expire
        float ParticleLifeTime;
        float ParticleLifeTimeVariation;
        glm::vec3 Velocity;
        glm::vec3 VelocityVariation;
        float SizeBegin;
        float SizeEnd;


        void startExpiring();
        void OnImGuiRender();
        static std::shared_ptr<Flame> LoadDataFromStrings(const std::vector<std::string>& data);
        void SaveSpecificDataToCSV(util::CSVWriter& csv);

        //PointLight abstract methods
        glm::vec3 GetPos() const override { return Position; }
        glm::vec3 GetColor() const override { return glm::vec3(1.f, 1.f, 1.f);}
    private:
        bool BuildingUp = true;
        bool Expiring = false; //if set true, the fire will start to expire
        bool Expired = false; //if set true, it is done completely
        float SecondsSinceEmit = 0.f; //internal emit timer
        float Timer = 0.f;

        sf::Sound Sound;
    };


    /** This class manages all the flames; it sends emit commands to the particle system*/
    class FireCreator{
    public:
        FireCreator(ParticleSystem& particleSystem, sound::SoundManager& soundManager);

        void onUpdate(float ts);
        void onImGuiRender();

        /** Adds a flame the list. Don't destroy Flames while FireCreator is alive!*/
        void startFlame(std::shared_ptr<Flame> flame);

        void startExpiringFlameOfName(const std::string& name);

        void clear(){
            m_Flames.clear();
            m_ParticleSystem.clear();
        }


    private:
        ParticleSystem& m_ParticleSystem;
        std::vector<std::shared_ptr<en::GLPictureTex>> m_Textures; //the variety of textures we use for fire
        ParticleProps m_BaseFlameProps;

        std::vector<std::shared_ptr<Flame>> m_Flames; //holds references to the flames

        float Emit_Frequency = 0.1f;


        sound::SoundManager& m_SoundManager;
        std::vector<std::shared_ptr<sf::SoundBuffer>> m_SoundBuffers;

        float m_SndVolume = 100.f;
        float m_SndAttenuation = 1.6f;
        float m_SndMinDistance = 5.f;


    };
}