//
//Created by vincent on 27.06.2021.
//

#include "particle/Smoke.h"


namespace particle{
        SmokeStream::SmokeStream(
                const char name[32],
                std::shared_ptr<en::Spline3D> spline,
                const glm::vec3& positionVariation,
                int particlesPerSecond,
                float speed,
                float speedVariation,
                float sizeBegin,
                float sizeEnd,
                float sizeVariationFactor,
                float buildUpTime,
                float expiringTime)
                : Spline(std::move(spline)),
                  PositionVariation(positionVariation),
                  Speed(speed),
                  SpeedVariation(speedVariation),
                  SizeBegin(sizeBegin),
                  SizeEnd(sizeEnd),
                  SizeVariationFactor(sizeVariationFactor),
                  ParticlesPerSecond(particlesPerSecond),
                  BuildUpTime(buildUpTime),
                  ExpiringTime(expiringTime)
        {
            strcpy_s(Name, name);
        }

        void SmokeStream::startExpiring(){
            if (Timer < BuildUpTime)
                Timer = (1.f-(Timer/BuildUpTime))*ExpiringTime;
            else
                Timer = 0.f;
            BuildingUp = false; Expiring = true;
        }


        void SmokeStream::OnImGuiRender(){
            ImGui::InputText("SmokeStream Name", Name, IM_ARRAYSIZE(Name));
            ImGui::DragFloat3("PositionVariation", &PositionVariation.x, 0.005f);
            ImGui::DragInt("ParticlesPerSecond", &ParticlesPerSecond, 1, 0, 999);
            ImGui::DragFloat("Speed", &Speed, 0.005f, 0.f, 999.f);
            ImGui::DragFloat("SpeedVariation", &SpeedVariation, 0.005f, 0.f, 999.f);
            ImGui::DragFloat("SizeBegin", &SizeBegin, 0.005f);
            ImGui::DragFloat("SizeEnd", &SizeEnd, 0.005f);
            ImGui::DragFloat("SizeVariationFactor", &SizeVariationFactor, 0.005f, 0.f, 1.f);
        }


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
                            props.Spline = smoke->Spline.get();
                            props.PositionVariation = smoke->PositionVariation;

                            //Now here I use the velocity variable to save the speed
                            // so the particle system will read it in the case that the spline is not nullptr
                            props.Velocity.x = smoke->Speed;
                            props.Velocity.y = smoke->SpeedVariation;

                            props.SizeBegin = smoke->SizeBegin;
                            props.SizeEnd = smoke->SizeEnd;
                            props.SizeVariationFactor = smoke->SizeVariationFactor;

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
        ImGui::Begin("Smoke");
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "Smoke Particle Props (General)");
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "#Particles:  %d", m_ParticleSystem.getActiveParticleCount());
        ImGui::ColorEdit4("ColorBegin", &m_BaseSmokeProps.ColorBegin.x);
        ImGui::ColorEdit4("ColorEnd", &m_BaseSmokeProps.ColorEnd.x);

        for (int i = 0; i < m_SmokeStreams.size(); ++i){
            ImGui::PushID(i);
            if (ImGui::TreeNode((std::string("SmokeStream ") + std::to_string(i)).c_str())) {
                m_SmokeStreams[i]->OnImGuiRender();
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::End();
    }


    void SmokeCreator::startSmokeStream(std::shared_ptr<SmokeStream> smoke){
        smoke->Expiring = false;
        smoke->Timer = 0.f;
        smoke->Expired = false;
        smoke->BuildingUp = true;
        m_SmokeStreams.emplace_back(smoke);
    }
}