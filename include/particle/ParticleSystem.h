// Simples Partikelsystem, von The Cherno übernommen: https://www.youtube.com/watch?v=GK0jHlv3e3w

#pragma once

#include "framework/common.hpp"
#include "util/Timestep.h"
#include "util/Random.h"
#include "framework/camera.hpp"
#include "Quad.h"

struct ParticleProps
{
	glm::vec3 Position;
	glm::vec3 Velocity;
    //for random variable r between -0.5 and 0.5, the velocity will be varied by r * VelocityVariation
	glm::vec3 VelocityVariation;
	glm::vec4 ColorBegin, ColorEnd;
	float SizeBegin, SizeEnd;
    //for random variable r between -0.5 and 0.5, the size will be varied by r * SizeVariation
	float SizeVariation;
	float LifeTime = 1.0f;
	float LifeTimeVariation = 0.f;
};

class ParticleSystem
{
public:
	ParticleSystem();

	void OnUpdate(util::TimeStep ts);
	void OnRender(glm::mat4& view_proj_matrix);

	//Creates a new particle given the properties
	void Emit(const ParticleProps& pProps);
private:
    static const unsigned int ParticlePoolSize = 3000;
	struct Particle
	{
		glm::vec3 Position;
		glm::vec3 Velocity;
		glm::vec4 ColorBegin, ColorEnd;
		float Rotation = 0.0f;
		float SizeBegin, SizeEnd;

		float LifeTime = 1.0f;
		float LifeRemaining = 0.0f;

		bool Active = false;
	};

    //Recycle-Pool of particles
	std::vector<Particle> m_ParticlePool;
	uint32_t m_PoolIndex = ParticlePoolSize-1;

    // The OpenGL Quad represented by two vertices
    Quad m_Quad;
};