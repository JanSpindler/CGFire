#include "particle/ParticleSystem.h"


namespace particle {
    ParticleSystem::ParticleSystem(uint32_t particlePoolSize,  const en::Camera& cam, bool additiveBlending)
            : m_Cam(cam),
            m_ParticlePoolSize(particlePoolSize),
              m_PoolIndex(m_ParticlePoolSize - 1),
              m_BatchRenderer(m_ParticlePoolSize, cam, additiveBlending){
        m_ParticlePool.resize(m_ParticlePoolSize);
    }

    void ParticleSystem::OnUpdate(float ts) {
        //For all particles update the remaining life-time, Position, Rotation...
        for (auto& particle : m_ParticlePool) {
            if (!particle.Active)
                continue;

            if (particle.LifeRemaining <= 0.0f) {
                particle.Active = false;
                particle.CameraDistance = -1.f;
                continue;
            }


            particle.LifeRemaining -= ts;
            particle.Velocity += ts * particle.GravityFactor * glm::vec3(0.f, -9.81f, 0.f);
            particle.Position += ts * particle.Velocity;

            auto dif = particle.Position - m_Cam.GetPos();
            particle.CameraDistance = glm::dot(dif, dif); //squared distance to camera, used to sort the particles

        }

        //Sorts all the particles by their distance to the camera.
        //Also will sort all inactive particles so make sure the pool is not too big
        std::sort(m_ParticlePool.begin(), m_ParticlePool.end());
    }

    void ParticleSystem::OnRender() {

        m_BatchRenderer.BeginBatch();

        for (size_t i = 0; i < m_ParticlePoolSize; i++){
            //The renderer will update transform and color of the individual vertices
            if (m_ParticlePool[i].Active)
                m_BatchRenderer.DrawParticle(m_ParticlePool[i]);
        }
        //Actually draws the particles
        this->m_BatchRenderer.EndBatch();
    }



    void ParticleSystem::Emit(const ParticleProps &pProps) {
        Particle &p = m_ParticlePool[m_PoolIndex];
        p.Active = true;

        p.Position = pProps.Position;
        p.Position.x += (util::Random::Float() - 0.5f) * pProps.PositionVariation.x;
        p.Position.y += (util::Random::Float() - 0.5f) * pProps.PositionVariation.y;
        p.Position.z += (util::Random::Float() - 0.5f) * pProps.PositionVariation.z;

        p.Velocity = pProps.Velocity;
        p.Velocity.x += (util::Random::Float() - 0.5f) * pProps.VelocityVariation.x;
        p.Velocity.y += (util::Random::Float() - 0.5f) * pProps.VelocityVariation.y;
        p.Velocity.z += (util::Random::Float() - 0.5f) * pProps.VelocityVariation.z;

        p.GravityFactor = pProps.GravityFactor;

        p.ColorBegin = pProps.ColorBegin;
        p.ColorEnd = pProps.ColorEnd;

        p.LifeTime = pProps.LifeTime + (util::Random::Float() - 0.5f) * pProps.LifeTimeVariation;
        p.LifeRemaining = p.LifeTime;

        p.SizeBegin = pProps.SizeBegin + (util::Random::Float() - 0.5f) * pProps.SizeVariation;
        p.SizeEnd = pProps.SizeEnd;

        p.Texture = pProps.Texture;
        p.TexCoord = pProps.TexCoord;
        p.TexCoordAnimFrames = pProps.TexCoordAnimFrames;

        auto dif = p.Position - m_Cam.GetPos();
        p.CameraDistance = glm::dot(dif, dif); //squared distance to camera

        m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
    }
}