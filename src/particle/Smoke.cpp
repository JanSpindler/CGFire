//
//Created by vincent on 27.06.2021.
//

#include "particle/Smoke.h"


namespace particle{

    SmokeCreator::SmokeCreator(ParticleSystem& particleSystem)
            : m_ParticleSystem(particleSystem)
    {

        m_Textures.emplace_back(std::make_shared<en::GLPictureTex>(
                    en::GLPictureTex::WrapMode::CLAMP_TO_BORDER,
                    en::GLPictureTex::FilterMode::NEAREST,
                    DATA_ROOT + "smoke/smoke_loop.png", true));


        m_ParticleSystem.InitializeTextures(m_Textures);

        m_BaseSmokeProps.ColorBegin = { 0.33, 0.33f, 0.33f, 0.5f };
        m_BaseSmokeProps.ColorEnd = { 1.f, 1.f, 1.f, 0.f };
        m_BaseSmokeProps.SizeBegin = 2.5f;
        m_BaseSmokeProps.SizeVariation = 1.5f;
        m_BaseSmokeProps.SizeEnd = 7.0f;
        m_BaseSmokeProps.TexCoordAnimFrames = {4, 4};
        m_BaseSmokeProps.TexLooped = true;

    }
    void SmokeCreator::onUpdate(float ts){

        //For all smoke streams emit new particles if the time has come
        for (auto& smoke : m_SmokeStreams) {
            float sizeFactor = 1.f; //will be set to less than 1 if the flame is building up or expiring
            if (smoke->BuildingUp){
                smoke->Timer += ts;
                sizeFactor = smoke->Timer / smoke->BuildUpTime;
                if (smoke->Timer > smoke->BuildUpTime) {
                    smoke->BuildingUp = false;
                    sizeFactor = 1.f;
                }
            }
            else if (smoke->Expiring){
                smoke->Timer += ts;
                sizeFactor = 1.f - (smoke->Timer / smoke->ExpiringTime);
                if (smoke->Timer > smoke->ExpiringTime){
                    smoke->Expired = true;
                    sizeFactor = 1.f;
                }
            }

            if (!smoke->Expired) {
                smoke->SecondsSinceEmit += ts;

                if (smoke->ParticlesPerSecond > 0) {
                    const float FREQUENCY = 1.f / (sizeFactor * static_cast<float>(smoke->ParticlesPerSecond));
                    if (smoke->SecondsSinceEmit > FREQUENCY) {
                        uint32_t numParticlesToEmit = static_cast<uint32_t>(smoke->SecondsSinceEmit / FREQUENCY);

                        ParticleProps props = m_BaseSmokeProps;

                        for (size_t i = 0; i < numParticlesToEmit; ++i) {
                            //Calculate ahead the position and velocity of the particles
                            // that should have been created in between the update calls
                            float dTime = i * FREQUENCY;

                            props.Spline = smoke->Spline.get();
                            props.PositionVariation = smoke->PositionVariation;

                            //Now here I use the velocity variable to save the speed
                            // so the particle system will read it in the case that the spline is not nullptr
                            props.Velocity.x = smoke->Speed;
                            props.Velocity.y = smoke->SpeedVariation;

                            //use random texture
                            uint32_t randTextureID = util::Random::Uint32(0,
                                                                          static_cast<uint32_t>(m_Textures.size() - 1));
                            props.Texture = m_Textures[randTextureID].get();

                            m_ParticleSystem.Emit(props);
                        }

                        smoke->SecondsSinceEmit -=
                                numParticlesToEmit * FREQUENCY; // do not set to 0, because save the rest time

                    }
                }
            }
        }

        //Remove expired smokes
        m_SmokeStreams.erase(std::remove_if(m_SmokeStreams.begin(), m_SmokeStreams.end(),
                                      [](std::shared_ptr<SmokeStream> s) { return s->Expired; }), m_SmokeStreams.end());



    }
    void SmokeCreator::onImGuiRender(){
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "Smoke Particle Props (General)");
        ImGui::ColorEdit4("ColorBegin", &m_BaseSmokeProps.ColorBegin.x);
        ImGui::ColorEdit4("ColorEnd", &m_BaseSmokeProps.ColorEnd.x);
        ImGui::SliderFloat("SizeBegin", &m_BaseSmokeProps.SizeBegin, 0, 15);
        ImGui::SliderFloat("SizeVariation", &m_BaseSmokeProps.SizeVariation, 0, m_BaseSmokeProps.SizeBegin);
        ImGui::SliderFloat("SizeEnd", &m_BaseSmokeProps.SizeEnd, 0, 15);



        for (int i = 0; i < m_SmokeStreams.size(); i++){
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "%s", ("smoke" + std::to_string(i)).c_str());
            ImGui::SliderFloat3("Position", &m_SmokeStreams[i]->Position.x, -50, 50);
            ImGui::SliderFloat3("PositionVariation", &m_SmokeStreams[i]->PositionVariation.x, 0, 10);
            ImGui::SliderFloat("Speed", &m_SmokeStreams[i]->Speed, 2, 50);
            ImGui::SliderFloat("SpeedVariation", &m_SmokeStreams[i]->SpeedVariation, 0, m_SmokeStreams[i]->Speed);
            ImGui::SliderInt("ParticlesPerSecond", &m_SmokeStreams[i]->ParticlesPerSecond, 0, 150);
        }
    }


    void SmokeCreator::startSmokeStream(std::shared_ptr<SmokeStream> smoke){
        smoke->Expiring = false;
        smoke->Timer = 0.f;
        smoke->Expired = false;
        smoke->BuildingUp = true;
        m_SmokeStreams.emplace_back(smoke);
    }
}