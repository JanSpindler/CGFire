//
//Created by vincent on 29.05.2021.
//
// Simple Particlesystem based on the Cherno (https://www.youtube.com/watch?v=GK0jHlv3e3w)
// and other particle system tutorials

#pragma once

#include "framework/common.hpp"
#include "util/Random.h"
#include "framework/camera.hpp"
#include "particle/ParticleSystemRenderer.h"
#include "particle/Particle.h"

namespace particle {


    class ParticleSystem {
    public:

        /**\param particlePoolSize should only approximately be as high as needed,
         * because each update has O(n) time plus O(nlogn) for sorting, n = particlePoolSize.*/
        explicit ParticleSystem(uint32_t particlePoolSize, const en::Camera& cam, bool additiveBlending = false);

        //Needs to be called with all the textures that will be used
        void InitializeTextures(std::vector<std::shared_ptr<en::GLPictureTex>>& textures){
            m_BatchRenderer.InitializeTextures(textures);
        }

        void OnUpdate(float ts);

        void OnRender();

        //Creates a new particle given the properties
        void Emit(const ParticleProps &pProps);

        //deactivate all particles
        void clear(){
            for (auto& p : m_ParticlePool)
                p.Active = false;
            m_ActiveParticleCount = 0;
        }

        uint32_t getActiveParticleCount() const { return m_ActiveParticleCount; }
    private:
        const unsigned int m_ParticlePoolSize;
        const en::Camera& m_Cam;

        //Recycle-Pool of particles
        std::vector<Particle> m_ParticlePool;
        Particle* m_NextUnusedParticle;

        uint32_t m_ActiveParticleCount = 0;

        //Renderer
        ParticleSystemRenderer m_BatchRenderer;



    };
}