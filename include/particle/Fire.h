//
//Created by vincent on 28.05.2021.
// Creation of Flames
#pragma once

#include "engine/render/GLTexture.hpp"
#include "particle/ParticleSystem.h"
#include <framework/imgui_util.hpp>

namespace particle{

    /*A flame of fire can be placed in the scene with individual properties.*/
    struct Flame{
        friend class FireCreator;
    public:
        explicit Flame(const glm::vec3& position,
              const glm::vec3& positionVariation = glm::vec3(1.f, 0.f, 1.f),
              int particlesPerEmit = 5,
              float buildUpTime = 5.f,
              float expiringTime = 5.f,
              float particleLifeTime = 1.f,
              float particleLifeTimeVariation = 0.2f)
        : Position(position),
          PositionVariation(positionVariation),
        ParticlesPerEmit(particlesPerEmit),
          BuildUpTime(buildUpTime),
          ExpiringTime(expiringTime),
          ParticleLifeTime(particleLifeTime),
          ParticleLifeTimeVariation(particleLifeTimeVariation)
        {

        }

        void startExpiring(){ Timer = 0.f; Expiring = true; }


        glm::vec3 Position;
        glm::vec3 PositionVariation;
        int ParticlesPerEmit;
        float ParticleLifeTime;
        float ParticleLifeTimeVariation;

    private:
        const float BuildUpTime; //the amount of time the fire takes to come to its peak
        const float ExpiringTime; //the amount of time the fire takes to expire

        bool BuildingUp = true;
        bool Expiring = false; //if set true, the fire will start to expire
        bool Expired = false; //if set true, it is done completely

        float SecondsSinceEmit = 0.f; //internal emit timer

        float Timer = 0.f;
    };


    /** This class manages all the flames; it sends emit commands to the particle system*/
    class FireCreator{
    public:
        FireCreator(ParticleSystem& particleSystem);

        void onUpdate(float ts);
        void onImGuiRender();

        /** Adds a flame the list. Don't destroy Flames while FireCreator is alive!*/
        void startFlame(Flame& flame);


    private:

        ParticleSystem& m_ParticleSystem;
        std::vector<std::shared_ptr<en::GLPictureTex>> m_Textures; //the variety of textures we use for fire
        ParticleProps m_BaseFlameProps;

        std::vector<Flame*> m_Flames; //holds references to the flames


    };
}