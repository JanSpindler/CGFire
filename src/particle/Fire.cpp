//
//Created by vincent on 28.05.2021.
//

#include "particle/Fire.h"


namespace particle{

    FireCreator::FireCreator(ParticleSystem& particleSystem)
            : m_ParticleSystem(particleSystem)
    {

        std::vector<std::string> sparkTextures(32);
        for (int i = 0; i < 32; i++){
            sparkTextures[i] = DATA_ROOT + "fire/fire(" + std::to_string(i) + ").png";
        }
        // load textures
        for (auto& file : sparkTextures){
            m_Textures.emplace_back(std::make_shared<en::GLPictureTex>(
                    en::GLPictureTex::WrapMode::CLAMP_TO_BORDER,
                    en::GLPictureTex::FilterMode::NEAREST,
                    file, true));
        }


        m_ParticleSystem.InitializeTextures(m_Textures);

        m_BaseFlameProps.Position = { 0.0f, 0.0f, 0.0f };
        m_BaseFlameProps.PositionVariation = { 1.0f, 0.0f, 1.0f };
        m_BaseFlameProps.Velocity = { 0.0f, 10.0f, 10.0f };
        m_BaseFlameProps.VelocityVariation = { 10.0f, 10.0f, 10.0f };
        m_BaseFlameProps.GravityFactor = 0.1f; // we barely want gravity to work on fire
        m_BaseFlameProps.ColorBegin = { 210 / 255.0f, 200 / 255.0f, 0 / 255.0f, 1.0f };
        m_BaseFlameProps.ColorEnd = { 250 / 255.0f, 0 / 255.0f, 0 / 255.0f, 0.3f };
        m_BaseFlameProps.SizeBegin = 1.f;
        m_BaseFlameProps.SizeVariation = 0.9f;
        m_BaseFlameProps.SizeEnd = 0.0f;
        m_BaseFlameProps.LifeTime = 1.f;
        m_BaseFlameProps.LifeTimeVariation = 0.2f;
        m_BaseFlameProps.TexCoordAnimFrames = {4, 4};

    }
    void FireCreator::onUpdate(float ts){

        //For all flames emit new particles if the time has come
        for (auto& flame : m_Flames) {
            flame->SecondsSinceEmit += ts;

            const float FREQUENCY = 0.1f;
            if (flame->SecondsSinceEmit > FREQUENCY + FREQUENCY*(util::Random::Float() - 0.5f)) //small random variation of frequency
            {
                ParticleProps props = m_BaseFlameProps;
                for (uint32_t i = 0; i < flame->ParticlesPerEmit; ++i) {
                    props.Position = flame->Position;

                    //use random texture
                    uint32_t randTextureID = util::Random::Uint32(0, static_cast<uint32_t>(m_Textures.size()-1));
                    props.Texture = m_Textures[randTextureID].get();

                    m_ParticleSystem.Emit(props);

                }
                flame->SecondsSinceEmit = 0;
            }
        }


        // UI
        ImGui::Begin("Flame Particle Props");
        ImGui::SliderFloat3("PositionVariation", &m_BaseFlameProps.PositionVariation.x, 0, 10);
        ImGui::SliderFloat3("Velocity", &m_BaseFlameProps.Velocity.x, 0, 10);
        ImGui::SliderFloat3("VelocityVariation", &m_BaseFlameProps.VelocityVariation.x, 0, 10);
        ImGui::SliderFloat("GravityFactor", &m_BaseFlameProps.GravityFactor, 0, 10);
        ImGui::ColorEdit4("ColorBegin", &m_BaseFlameProps.ColorBegin.x);
        ImGui::ColorEdit4("ColorEnd", &m_BaseFlameProps.ColorEnd.x);
        ImGui::SliderFloat("SizeBegin", &m_BaseFlameProps.SizeBegin, 0, 100);
        ImGui::SliderFloat("SizeVariation", &m_BaseFlameProps.SizeVariation, 0, 10);
        ImGui::SliderFloat("SizeEnd", &m_BaseFlameProps.SizeEnd, 0, 10);
        ImGui::SliderFloat("LifeTime", &m_BaseFlameProps.LifeTime, 0, 10);
        ImGui::SliderFloat("LifeTimeVariation", &m_BaseFlameProps.LifeTimeVariation, 0, 10);
        ImGui::End();
    }
    std::shared_ptr<Flame> FireCreator::createFlame(const glm::vec3& position, int particlesPerEmit){

        m_Flames.emplace_back(std::make_shared<Flame>());
        Flame& flame = *m_Flames.back();
        flame.Position = position;
        flame.ParticlesPerEmit = particlesPerEmit;

        return m_Flames.back();
    }
}