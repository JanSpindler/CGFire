//
//Created by vincent on 28.05.2021.
// Calls the particle system to create fire
#pragma once

#include "engine/GLTexture.hpp"
#include "particle/ParticleSystem.h"
#include <framework/imgui_util.hpp>

namespace particle{
    struct Flame{
        glm::vec3 Position;
        float SecondsSinceEmit = 0.f;
        int ParticlesPerEmit = 1;

    };

    class FireCreator{
    public:
        FireCreator(ParticleSystem& particleSystem);

        void onUpdate(float ts);

        /**A flame is a point at \a position that emits \a particlesPerEmit of fire per emit period*/
        std::shared_ptr<Flame> createFlame(const glm::vec3& position, int particlesPerEmit = 1);
    private:
        ParticleSystem& m_ParticleSystem;
        std::vector<std::shared_ptr<en::GLPictureTex>> m_Textures; //the variety of textures we use for fire
        ParticleProps m_BaseFlameProps;
        std::vector<std::shared_ptr<Flame>> m_Flames;
    };
}