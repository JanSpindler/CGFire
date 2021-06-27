#include "particle/ParticleSystem.h"


namespace particle {
    ParticleSystem::ParticleSystem(uint32_t particlePoolSize,  const en::Camera& cam, bool additiveBlending)
            : m_Cam(cam),
            m_ParticlePoolSize(particlePoolSize),
              m_BatchRenderer(m_ParticlePoolSize, cam, additiveBlending){
        m_ParticlePool.resize(m_ParticlePoolSize);
        for (size_t i = 0; i < m_ParticlePoolSize-1; ++i){
            m_ParticlePool[i].NextParticle = &m_ParticlePool[i+1];
        }
        m_ParticlePool[m_ParticlePoolSize-1].NextParticle = &m_ParticlePool[0];
        m_NextUnusedParticle = &m_ParticlePool[0];
    }

    void ParticleSystem::OnUpdate(float ts) {
        //For all particles update the remaining life-time, Position, Rotation...
        for (auto& particle : m_ParticlePool) {
            if (!particle.Active)
                continue;


            if (particle.Spline == nullptr) { //e.g. fire and waterjet don't use a spline
                particle.LifeRemaining -= ts;
                if (particle.LifeRemaining <= 0.0f) { //this case only happens
                    particle.Active = false;
                    particle.CameraDistance = -1.f;
                    continue;
                }
                particle.Velocity += ts * particle.GravityFactor * glm::vec3(0.f, -9.81f, 0.f);
                particle.Position += ts * particle.Velocity;
            }
            else{ //smoke does use a spline
                // when using a spline, we want to know how long the particle has been alive
                particle.LifeRemaining += ts; //so LifeRemaining here means Life Time Lived

                //in Velocity.x is the speed of the smoke particle
                float t = (particle.Velocity.x * particle.LifeRemaining) / particle.Spline->GetLength();
                if (t > 1.f){
                    particle.Active = false;
                    particle.CameraDistance = -1.f;
                    continue;
                }

                float exactPoint = t * static_cast<float>(particle.Spline->GetPointCount());
                exactPoint = std::clamp(exactPoint, 0.f, static_cast<float>(particle.Spline->GetPointCount()-1));


                particle.Position = glm::lerp(particle.Spline->GetPoints()[static_cast<int>(std::floor(exactPoint))],
                                              particle.Spline->GetPoints()[static_cast<int>(std::ceil(exactPoint))],
                                              exactPoint - std::floor(exactPoint));
                particle.LifeTime = t; // save for the ParticleSystemRenderer
            }

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
        Particle &p = *m_NextUnusedParticle;
        m_NextUnusedParticle = p.NextParticle;

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
        p.TexLooped = pProps.TexLooped;

        auto dif = p.Position - m_Cam.GetPos();
        p.CameraDistance = glm::dot(dif, dif); //squared distance to camera

        // Special case that the particles shall follow the spline
        p.Spline = pProps.Spline;
        if (p.Spline != nullptr) {
            //now what we actually do here is save the elapsed life time
            //because when using a spline, there is no "life remaining."
            //The particle flows along the spline at given speed instead.
            // dTime was saved there, because velocity is unused
            p.LifeRemaining = 0.f;

            //in x coord the speed in y coord, the speed variation is stored
            p.Velocity.x = pProps.Velocity.x + (util::Random::Float() - 0.5f) * pProps.Velocity.y;
        }
    }
}