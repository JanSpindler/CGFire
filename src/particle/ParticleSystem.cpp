#include "particle/ParticleSystem.h"


#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>


ParticleSystem::ParticleSystem()
{
	m_ParticlePool.resize(ParticlePoolSize);
}

void ParticleSystem::OnUpdate(util::TimeStep ts)
{
	for (auto& particle : m_ParticlePool)
	{
		if (!particle.Active)
			continue;

		if (particle.LifeRemaining <= 0.0f)
		{
			particle.Active = false;
			continue;
		}

		particle.LifeRemaining -= ts;
		particle.Position += particle.Velocity * (float)ts;
		particle.Rotation += 0.01f * ts;
	}
}

void ParticleSystem::OnRender(glm::mat4& view_proj_matrix)
{
	//draw all alive particles
	for (auto& particle : m_ParticlePool)
	{
		if (!particle.Active)
			continue;


        // change quad color and transform

		float life = particle.LifeRemaining / particle.LifeTime;
		glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);
		//color.a = color.a * life;

		float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), { particle.Position.x, particle.Position.y, 0 })
			* glm::rotate(glm::mat4(1.0f), particle.Rotation, { 0.0f, 0.0f, 1.0f })
			* glm::scale(glm::mat4(1.0f), { size, size, 1 });

		// draw the squad
		this->m_Quad.OnRender(view_proj_matrix, transform, color);
	}
}

void ParticleSystem::Emit(const ParticleProps& pProps)
{
	Particle& p = m_ParticlePool[m_PoolIndex];
    p.Active = true;
    p.Position = pProps.Position + pProps.PositionVariation * (util::Random::Float() - 0.5f);
    p.Rotation = util::Random::Float() * 2.0f * glm::pi<float>(); //Random rotation

	// Velocity
	p.Velocity = pProps.Velocity;
    p.Velocity.x += pProps.VelocityVariation.x * (util::Random::Float() - 0.5f);
    p.Velocity.y += pProps.VelocityVariation.y * (util::Random::Float() - 0.5f);
    p.Velocity.z += 0;//particleProps.VelocityVariation.z * (util::Random::Float() - 0.5f);

	// Color
	p.ColorBegin = pProps.ColorBegin;
    p.ColorEnd = pProps.ColorEnd;

	// Lifespan
	p.LifeTime = pProps.LifeTime + pProps.LifeTimeVariation * (util::Random::Float() - 0.5f);
    p.LifeRemaining = p.LifeTime;

	// size
	p.SizeBegin = pProps.SizeBegin + pProps.SizeVariation * (util::Random::Float() - 0.5f);
    p.SizeEnd = pProps.SizeEnd;

	m_PoolIndex = --m_PoolIndex % m_ParticlePool.size();
}
