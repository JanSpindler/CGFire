// Simples Partikelsystem, von The Cherno übernommen: https://www.youtube.com/watch?v=GK0jHlv3e3w

#pragma once

#include "framework/common.hpp"
#include "util/Timestep.h"
#include "util/Random.h"
#include "framework/camera.hpp"
#include "particle/ParticleSystemRenderer.h"
#include "particle/Particle.h"

namespace particle {


    class ParticleSystem {
    public:

        /**\param particlePoolSize should only approximately be as high as needed,
         * because each update has O(n) time plus O(nlogn) for sorting, n = particlePoolSize.*/
        explicit ParticleSystem(uint32_t particlePoolSize, const en::Camera& cam);

        void initializeTextures(std::vector<std::shared_ptr<en::GLPictureTex>>& textures){
            m_BatchRenderer.initializeTextures(textures);
        }

        void OnUpdate(float ts);

        void OnRender();

        //Creates a new particle given the properties
        void Emit(const ParticleProps &pProps);

    private:
        const unsigned int m_ParticlePoolSize;
        const en::Camera& m_Cam;

        //Recycle-Pool of particles
        std::vector<Particle> m_ParticlePool;
        uint32_t m_PoolIndex;

        //Renderer
        ParticleSystemRenderer m_BatchRenderer;



    };
}