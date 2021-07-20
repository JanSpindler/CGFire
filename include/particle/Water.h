//
//Created by vincent on 20.06.2021.
// Calls the particle system to create a water
#pragma once

#include "engine/render/GLTexture.hpp"
#include "particle/ParticleSystem.h"
#include <framework/imgui_util.hpp>
#include <util/CSVWriter.h>
#include <sound/SoundManager.h>

namespace en{
    class RenderObj;
}
namespace particle{

    class WaterJet{
        friend class WaterCreator;
    public:

        explicit WaterJet(
                        const char name[32] = "WaterNoName",
                        const glm::vec3& position = glm::vec3(2.f, 0.f, 1.f),
                        const glm::vec3& positionVariation = glm::vec3(0.5f, 0.5f, 0.5f),
                        const glm::vec3& waterDirection = glm::vec3(1.f, 0.5f, 0.f),
                        float speed = 100.f,
                        float speedVariationFactor = 0.1f,
                       int particlesPerSecond = 200,
                       float buildUpTime = 3.f,
                       float expiringTime = 1.f,
                       float particleLifeTime = 5.f,
                       float particleLifeTimeVariation = 0.2f,
                       float sizeBegin = 1.f,
                       float sizeEnd = 3.5f);


        ~WaterJet(){
            Sound.stop();
        }

        //Relevant Data
        char Name[32];
        glm::vec3 Position;
        glm::vec3 PositionVariation;
        glm::vec3 WaterDirection; // the direction of the water coming out of the source
        float Speed;
        float SpeedVariationFactor; //percentage-range of varying speed
        int ParticlesPerSecond;
        float BuildUpTime; //the amount of time the fire takes to come to its peak
        float ExpiringTime; //the amount of time the fire takes to expire
        float ParticleLifeTime;
        float ParticleLifeTimeVariation;
        float SizeBegin;
        float SizeEnd;

        void startExpiring();
        void OnImGuiRender();
        static std::shared_ptr<WaterJet> LoadDataFromStrings(const std::vector<std::string>& data);
        void SaveSpecificDataToCSV(util::CSVWriter& csv) const;
    private:
        bool BuildingUp = true;
        bool Expiring = false; //if set true, the fire will start to expire
        bool Expired = false; //if set true, it is done completely
        float SecondsSinceEmit = 0.f; //internal emit timer
        float Timer = 0.f;

        sf::Sound Sound;
    };

    class WaterCreator{
    public:
        WaterCreator(ParticleSystem& particleSystem, sound::SoundManager& soundManager);

        void onUpdate(float ts);
        void onImGuiRender();

        /** Adds a water jet to the list. Don't destroy WaterJets while WaterCreator is alive!*/
        void startWaterJet(std::shared_ptr<WaterJet> waterJet);

        void startExpiringWaterJetOfName(const std::string& name);

        void clear(){
            m_WaterJets.clear();
            m_ParticleSystem.clear();
            m_WaterJetToObjectConnections.clear();
        }



        void ConnectWaterJetRelativeToObject(const std::string& waterJetName, en::RenderObj* obj,
                                             const glm::vec3& relativePos, const glm::quat& quaternion);

    private:
        ParticleSystem& m_ParticleSystem;
        std::vector<std::shared_ptr<en::GLPictureTex>> m_Textures; //the variety of textures we use for water
        ParticleProps m_BaseWaterJetProps;
        std::vector<std::shared_ptr<WaterJet>> m_WaterJets; //references to the water jets

        WaterJet* GetWaterJetByName(const std::string& name);


        typedef std::tuple<WaterJet*, en::RenderObj*, glm::vec3, glm::quat> WaterJetToObjectConnection_t;
        std::list<WaterJetToObjectConnection_t> m_WaterJetToObjectConnections;
        void UpdateWaterJetToObjectConnections();


        sound::SoundManager& m_SoundManager;
        std::vector<std::shared_ptr<sf::SoundBuffer>> m_SoundBuffers;

        float m_SndVolume = 50.f;
        float m_SndAttenuation = 1.6f;
        float m_SndMinDistance = 5.f;
    };
}