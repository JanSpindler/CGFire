//
// Created by JS on 03/06/2021.
//

#ifndef CGFIRE_FIRE_H
#define CGFIRE_FIRE_H

#include "engine/render/GLTexture.hpp"
#include "particle/ParticleSystem.h"
#include <framework/imgui_util.hpp>

namespace particle{

    class FireCreator{
    public:
        FireCreator(ParticleSystem& particleSystem);

        void onUpdate(float ts);

        /**A flame is a point at \a position that emits \a particlesPerEmit of fire per emit period*/
        void createFlame(const glm::vec3& position, uint32_t particlesPerEmit = 1);
    private:
        ParticleSystem& m_ParticleSystem;
        std::vector<std::shared_ptr<en::GLPictureTex>> m_Textures; //the variety of textures we use for fire
        ParticleProps m_BaseFlameProps;

        struct Flame{
            glm::vec3 Position;
            float SecondsSinceEmit = 0.f;
            uint32_t ParticlesPerEmit = 1;

        };
        std::vector<std::shared_ptr<Flame>> m_Flames;



    };
}

#endif //CGFIRE_FIRE_H
