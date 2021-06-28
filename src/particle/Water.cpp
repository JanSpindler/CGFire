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

        m_BaseWaterJetProps.VelocityVariation = {0.f, 0.f, 0.f};
        m_BaseWaterJetProps.GravityFactor = 10.f;
        m_BaseWaterJetProps.ColorBegin = { 255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f };
        m_BaseWaterJetProps.ColorEnd = { 255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f };
        m_BaseWaterJetProps.SizeBegin = 1.f;
        m_BaseWaterJetProps.SizeVariation = 0.9f;
        m_BaseWaterJetProps.SizeEnd = 0.7f;
        m_BaseWaterJetProps.TexCoordAnimFrames = {1, 1};

    }
    void WaterCreator::onUpdate(float ts){
        //For all water jets emit new particles
        for (auto& waterJet : m_WaterJets) {
            float sizeFactor = 1.f; //will be set to less than 1 if the waterJet is building up or expiring
            if (waterJet->BuildingUp){
                waterJet->Timer += ts;
                sizeFactor = waterJet->Timer / waterJet->BuildUpTime;
                if (waterJet->Timer > waterJet->BuildUpTime) {
                    waterJet->BuildingUp = false;
                    sizeFactor = 1.f;
                }
            }
            else if (waterJet->Expiring){
                waterJet->Timer += ts;
                sizeFactor = 1.f - (waterJet->Timer / waterJet->ExpiringTime);
                if (waterJet->Timer > waterJet->ExpiringTime){
                    waterJet->Expired = true;
                    sizeFactor = 1.f;
                }
            }


            if (!waterJet->Expired) {
                waterJet->SecondsSinceEmit += ts;

                if (waterJet->ParticlesPerSecond > 0) {

                    const float FREQUENCY = 1.f / (sizeFactor * static_cast<float>(waterJet->ParticlesPerSecond));
                    if (waterJet->SecondsSinceEmit > FREQUENCY) {
                        uint32_t numParticlesToEmit = static_cast<uint32_t>(waterJet->SecondsSinceEmit / FREQUENCY);
                        ParticleProps props = m_BaseWaterJetProps;

                        for (size_t i = 0; i < numParticlesToEmit; ++i) {
                            //Calculate ahead the position and velocity of the particles
                            // that should have been created in between the update calls
                            float dTime = i * FREQUENCY;
                            props.Velocity = sizeFactor
                                    * ((util::Random::Float() - 0.5f) * waterJet->SpeedVariationFactor + 1)
                                    * waterJet->Speed
                                    * waterJet->WaterDirection
                                    + dTime * props.GravityFactor * glm::vec3(0.f, -9.81f, 0.f);
                            props.Position = waterJet->Position + dTime * props.Velocity;
                            props.PositionVariation = waterJet->PositionVariation;

                            props.LifeTime = waterJet->ParticleLifeTime;
                            props.LifeTimeVariation = waterJet->ParticleLifeTimeVariation;

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
        //Remove expired water jets
        m_WaterJets.erase(std::remove_if(m_WaterJets.begin(), m_WaterJets.end(),
                                         [](std::shared_ptr<WaterJet> f) { return f->Expired; }), m_WaterJets.end());

    }

    void WaterCreator::onImGuiRender(){
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "WaterJet Particle Props (General)");
        ImGui::SliderFloat("GravityFactor", &m_BaseWaterJetProps.GravityFactor, 0, 10);
        ImGui::ColorEdit4("ColorBegin", &m_BaseWaterJetProps.ColorBegin.x);
        ImGui::ColorEdit4("ColorEnd", &m_BaseWaterJetProps.ColorEnd.x);
        ImGui::SliderFloat("SizeBegin", &m_BaseWaterJetProps.SizeBegin, 0, 10);
        ImGui::SliderFloat("SizeVariation", &m_BaseWaterJetProps.SizeVariation, 0, m_BaseWaterJetProps.SizeBegin);
        ImGui::SliderFloat("SizeEnd", &m_BaseWaterJetProps.SizeEnd, 0, 10);

        for (int i = 0; i < m_WaterJets.size(); i++) {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", ("WaterJet" + std::to_string(i)).c_str());
            ImGui::SliderFloat3("Position", &m_WaterJets[i]->Position.x, -50, 50);
            ImGui::SliderFloat3("PositionVariation", &m_WaterJets[i]->PositionVariation.x, -10, 10);
            ImGui::SliderFloat3("WaterDirection", &m_WaterJets[i]->WaterDirection.x, -1, 1);
            ImGui::SliderFloat("Speed", &m_WaterJets[i]->Speed, 0, 300);
            ImGui::SliderFloat("SpeedVariation", &m_WaterJets[i]->SpeedVariationFactor, 0, 1);
            ImGui::SliderInt("ParticlesPerSecond", &m_WaterJets[i]->ParticlesPerSecond, 0, 1000);
            ImGui::SliderFloat("ParticleLifeTime", &m_WaterJets[i]->ParticleLifeTime, 0, 10);
            ImGui::SliderFloat("ParticleLifeTimeVariation", &m_WaterJets[i]->ParticleLifeTimeVariation, 0, m_WaterJets[i]->ParticleLifeTime);
        }

    }

    void WaterCreator::startWaterJet(std::shared_ptr<WaterJet> waterJet){
        waterJet->Expiring = false;
        waterJet->Timer = 0.f;
        waterJet->Expired = false;
        waterJet->BuildingUp = true;
        m_WaterJets.emplace_back(waterJet);
    }
}