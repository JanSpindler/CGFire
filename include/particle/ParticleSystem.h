// Simples Partikelsystem, von The Cherno übernommen: https://www.youtube.com/watch?v=GK0jHlv3e3w

#pragma once

#include "framework/common.hpp"
#include "util/Timestep.h"
#include "util/Random.h"
#include "framework/camera.hpp"
#include "framework/shader.hpp"

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
};

class ParticleSystem
{
public:
	ParticleSystem();

	void OnUpdate(util::TimeStep ts);
	void OnRender(camera& cam);

	//Creates a new particle given the properties
	void Emit(const ParticleProps& particleProps);
private:
    static const unsigned int ParticlePoolSize = 1000;
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
	std::vector<Particle> m_ParticlePool; //Der Pool spart Speicherallokationsaufwand (Recyclen von Partikeln)
	uint32_t m_PoolIndex = ParticlePoolSize-1;

	GLuint m_QuadVA = 0;
    GLuint m_ParticleShader;

	GLint m_ParticleShaderViewProj, m_ParticleShaderTransform, m_ParticleShaderColor;
};