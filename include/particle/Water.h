//
//Created by vincent on 20.06.2021.
// Calls the particle system to create a water
#pragma once

#include "engine/render/GLTexture.hpp"
#include "particle/ParticleSystem.h"
#include <framework/imgui_util.hpp>

namespace particle{

    struct WaterJet{
        glm::vec3 Position;
        glm::vec3 StartGradient;
        float SecondsSinceEmit = 0.f;
        int ParticlesPerSecond = 1;

    };

    class WaterCreator{
    public:
        WaterCreator(ParticleSystem& particleSystem);

        void onUpdate(float ts);

        /**A Water Jet is a point at \a pos that emits \a particlesPerSecond of water per second in direction dir*/
        std::shared_ptr<WaterJet> createWaterJet(const glm::vec3& pos, const glm::vec3& dir, uint32_t particlesPerSecond = 1);
    private:
        ParticleSystem& m_ParticleSystem;
        std::vector<std::shared_ptr<en::GLPictureTex>> m_Textures; //the variety of textures we use for water
        ParticleProps m_BaseWaterJetProps;
        std::vector<std::shared_ptr<WaterJet>> m_WaterJets;

    };
}