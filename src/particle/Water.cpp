//
//Created by vincent on 20.06.2021.
//

#include "particle/Water.h"


namespace particle{

    WaterCreator::WaterCreator(ParticleSystem& particleSystem)
            : m_ParticleSystem(particleSystem)
    {

        std::vector<std::string> waterTextures(17);
        for (int i = 0; i < 17; i++){
            waterTextures[i] = DATA_ROOT + "water/water" + std::to_string(i) + ".png";
        }
        // load textures
        for (auto& file : waterTextures){
            m_Textures.emplace_back(std::make_shared<en::GLPictureTex>(
                    en::GLPictureTex::WrapMode::CLAMP_TO_BORDER,
                    en::GLPictureTex::FilterMode::NEAREST,
                    file, true));
        }


        m_ParticleSystem.InitializeTextures(m_Textures);

        m_BaseWaterJetProps.Position = { 0.0f, 0.0f, 0.0f };
        m_BaseWaterJetProps.PositionVariation = { 1.0f, 1.0f, 1.0f };
        m_BaseWaterJetProps.Velocity = { 0.0f, 0.0f, 0.0f };
        m_BaseWaterJetProps.VelocityVariation = { 1.0f, 1.0f, 1.0f };
        m_BaseWaterJetProps.GravityFactor = 1.f;
        m_BaseWaterJetProps.ColorBegin = { 255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f };
        m_BaseWaterJetProps.ColorEnd = { 255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f };
        m_BaseWaterJetProps.SizeBegin = 1.f;
        m_BaseWaterJetProps.SizeVariation = 0.9f;
        m_BaseWaterJetProps.SizeEnd = 0.7f;
        m_BaseWaterJetProps.LifeTime = 3.f;
        m_BaseWaterJetProps.LifeTimeVariation = 0.2f;
        m_BaseWaterJetProps.TexCoordAnimFrames = {1, 1};

    }
    void WaterCreator::onUpdate(float ts){

        //For all flames emit new particles if the time has come
        for (auto& waterJet : m_WaterJets) {
            if (waterJet->TurnedOn) {
                waterJet->SecondsSinceEmit += ts;

                if (waterJet->ParticlesPerSecond > 0) {
                    const float FREQUENCY = 1.f / static_cast<float>(waterJet->ParticlesPerSecond);
                    if (waterJet->SecondsSinceEmit > FREQUENCY) {
                        uint32_t numParticlesToEmit = static_cast<uint32_t>(waterJet->SecondsSinceEmit / FREQUENCY);
                        for (size_t i = 0; i < numParticlesToEmit; ++i) {
                            ParticleProps props = m_BaseWaterJetProps;

                            //Calculate ahead the position and velocity of the particles
                            // that should have been created in between the update calls
                            float dTime = i * FREQUENCY;
                            props.Velocity =
                                    waterJet->StartGradient + dTime * props.GravityFactor * glm::vec3(0.f, -9.81f, 0.f);
                            props.Position = waterJet->Position + dTime * props.Velocity;

                            //use random texture
                            uint32_t randTextureID = util::Random::Uint32(0,
                                                                          static_cast<uint32_t>(m_Textures.size() - 1));
                            props.Texture = m_Textures[randTextureID].get();

                            m_ParticleSystem.Emit(props);
                        }

                        waterJet->SecondsSinceEmit -=
                                numParticlesToEmit * FREQUENCY; // do not set to 0, because save the rest time
                    }
                }
            }
        }


        ImGui::Begin("WaterJet Particle Props");
        ImGui::SliderFloat3("PositionVariation", &m_BaseWaterJetProps.PositionVariation.x, 0, 10);
        ImGui::SliderFloat3("VelocityVariation", &m_BaseWaterJetProps.VelocityVariation.x, 0, 10);
        ImGui::SliderFloat("GravityFactor", &m_BaseWaterJetProps.GravityFactor, 0, 10);
        ImGui::ColorEdit4("ColorBegin", &m_BaseWaterJetProps.ColorBegin.x);
        ImGui::ColorEdit4("ColorEnd", &m_BaseWaterJetProps.ColorEnd.x);
        ImGui::SliderFloat("SizeBegin", &m_BaseWaterJetProps.SizeBegin, 0, 100);
        ImGui::SliderFloat("SizeVariation", &m_BaseWaterJetProps.SizeVariation, 0, 10);
        ImGui::SliderFloat("SizeEnd", &m_BaseWaterJetProps.SizeEnd, 0, 10);
        ImGui::SliderFloat("LifeTime", &m_BaseWaterJetProps.LifeTime, 0, 10);
        ImGui::SliderFloat("LifeTimeVariation", &m_BaseWaterJetProps.LifeTimeVariation, 0, 10);
        ImGui::End();

    }
    std::shared_ptr<WaterJet> WaterCreator::createWaterJet(const glm::vec3& pos, const glm::vec3& dir, uint32_t particlesPerSecond){

        m_WaterJets.emplace_back(std::make_shared<WaterJet>());
        WaterJet& waterJet = *m_WaterJets.back();
        waterJet.Position = pos;
        waterJet.StartGradient = dir;
        waterJet.ParticlesPerSecond = particlesPerSecond;

        return m_WaterJets.back();
    }
}