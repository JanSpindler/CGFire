//
// Created by JS on 28/05/2021.
//

#ifndef CGFIRE_PARTICLESYSTEM_HPP
#define CGFIRE_PARTICLESYSTEM_HPP

#include "Quad.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace en
{
    struct ParticleProps
    {
        glm::vec3 pos_;
        glm::vec3 vel_;

        glm::vec3 velVariation_;
        glm::vec4 colorBegin_;
        glm::vec4 colorEnd_;
        float sizeBegin_;
        float sizeEnd_;

        float sizeVariation_;
        float lifeTime_ = 1.0f;
        float lifeTimeVariation_ = 0.0f;
    };

    class ParticleSystem
    {
    public:
        ParticleSystem();

        void OnUpdate(float deltaTime);
        void OnRender(glm::mat4& view_proj_matrix);

        void Emit(const ParticleProps& props);

    private:
        struct Particle
        {
            glm::vec3 pos_;
            glm::vec3 vel_;
            glm::vec4 colorBegin_;
            glm::vec4 colorEnd_;
            float rotation_ = 0.0f;
            float sizeBegin_;
            float sizeEnd_;

            float lifeTime_ = 1.0f;
            float lifeRemaining_ = 0.0f;

            bool active_ = false;
        };

        static const unsigned int particlePoolSize_ = 3000;

        std::vector<Particle> particlePool_;
        unsigned int poolIndex_ = particlePoolSize_ - 1;

        Quad quad_;
    };
}

#endif //CGFIRE_PARTICLESYSTEM_HPP
