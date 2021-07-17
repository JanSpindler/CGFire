//
//Created by vincent on 28.05.2021.
//

#include "particle/Fire.h"

namespace particle{

    Flame::Flame(const char name[32],
                 const glm::vec3& position,
          const glm::vec3& positionVariation,
          int particlesPerEmit,
          float buildUpTime,
          float expiringTime,
          float particleLifeTime,
          float particleLifeTimeVariation)
            : PointLight(1.f),
              Position(position),
              PositionVariation(positionVariation),
              ParticlesPerEmit(particlesPerEmit),
              BuildUpTime(buildUpTime),
              ExpiringTime(expiringTime),
              ParticleLifeTime(particleLifeTime),
              ParticleLifeTimeVariation(particleLifeTimeVariation)
    {
        strcpy_s(Name, name);
    }


    void Flame::startExpiring(){
        if (Timer < BuildUpTime)
            Timer = (1.f-(Timer/BuildUpTime))*ExpiringTime;
        else
            Timer = 0.f;
        BuildingUp = false; Expiring = true;
    }

    void Flame::OnImGuiRender(){
        ImGui::InputText("Flame Name", Name, IM_ARRAYSIZE(Name));
        ImGui::DragFloat3("Position", &Position.x, 0.05f);
        ImGui::DragFloat3("PositionVariation", &PositionVariation.x, 0.05f);
        ImGui::DragInt("ParticlesPerEmit", &ParticlesPerEmit, 1, 0, 999);
        ImGui::DragFloat("BuildUpTime", &BuildUpTime, 0.01f, 0.f, 10.f);
        ImGui::DragFloat("ExpiringTime", &ExpiringTime, 0.01f, 0.f, 10.f);
        ImGui::DragFloat("ParticleLifeTime", &ParticleLifeTime, 0.01f, 0.f, 999.f);
        ImGui::DragFloat("ParticleLifeTimeVariation", &ParticleLifeTimeVariation, 0.05f, 0.f, 999.f);
    }

    FireCreator::FireCreator(ParticleSystem& particleSystem)
            : m_ParticleSystem(particleSystem)
    {

        std::vector<std::string> sparkTextures(32);
        for (size_t i = 0; i < 32; i++){
            sparkTextures[i] = DATA_ROOT + "fire/fire(" + std::to_string(i) + ").png";
        }

        // load textures
        for (auto& file : sparkTextures){
            m_Textures.emplace_back(std::make_shared<en::GLPictureTex>(
                    en::GLPictureTex::WrapMode::CLAMP_TO_BORDER,
                    en::GLPictureTex::FilterMode::LINEAR,
                    file, true));
        }


        m_ParticleSystem.InitializeTextures(m_Textures);

        m_BaseFlameProps.Velocity = { 0.0f, 8.0f, 0.0f };
        m_BaseFlameProps.VelocityVariation = { 8.0f, 10.0f, 8.0f };
        m_BaseFlameProps.GravityFactor = 0.1f; // we barely want gravity to work on fire
        m_BaseFlameProps.ColorBegin = { 1.f, 1.f, 1.f, 1.0f };
        m_BaseFlameProps.ColorEnd = { 1.f, 1.f, 1.f, 0.3f };
        m_BaseFlameProps.SizeBegin = 1.f;
        m_BaseFlameProps.SizeVariationFactor = 0.1f;
        m_BaseFlameProps.SizeEnd = 0.8f;
        m_BaseFlameProps.TexCoordAnimFrames = {4, 4};

    }
    void FireCreator::onUpdate(float ts){

        //For all flames emit new particles if the time has come
        for (auto& flame : m_Flames) {
            float flameSizeFactor = 1.f; //will be set to less than 1 if the flame is building up or expiring
            if (flame->BuildingUp){
                flame->Timer += ts;
                flameSizeFactor = flame->Timer / flame->BuildUpTime;
                if (flame->Timer > flame->BuildUpTime) {
                    flame->BuildingUp = false;
                    flameSizeFactor = 1.f;
                }
            }
            else if (flame->Expiring){
                flame->Timer += ts;
                flameSizeFactor = 1.f - (flame->Timer / flame->ExpiringTime);
                if (flame->Timer > flame->ExpiringTime){
                    flame->Expired = true;
                    flameSizeFactor = 1.f;
                }
            }

            if (!flame->Expired) {
                flame->SecondsSinceEmit += ts;

                if (flame->SecondsSinceEmit >
                        Emit_Frequency + Emit_Frequency * (util::Random::Float() - 0.5f)) //small random variation of frequency
                {
                    ParticleProps props = m_BaseFlameProps;
                    props.Position = flame->Position;
                    props.PositionVariation = flameSizeFactor * flame->PositionVariation;
                    props.LifeTime = flameSizeFactor * flame->ParticleLifeTime;
                    props.LifeTimeVariation = flameSizeFactor * flame->ParticleLifeTimeVariation;

                    props.SizeBegin = flameSizeFactor * m_BaseFlameProps.SizeBegin;
                    props.SizeEnd = flameSizeFactor * m_BaseFlameProps.SizeEnd;
                    props.SizeVariationFactor = m_BaseFlameProps.SizeVariationFactor;

                    uint32_t numParticlesEmitted = static_cast<uint32_t>(
                            std::ceil(flameSizeFactor * static_cast<float>(flame->ParticlesPerEmit)));
                    for (uint32_t i = 0; i < numParticlesEmitted; ++i) {
                        //use random texture
                        uint32_t randTextureID = util::Random::Uint32(0, static_cast<uint32_t>(m_Textures.size() - 1));
                        props.Texture = m_Textures[randTextureID].get();

                        m_ParticleSystem.Emit(props);

                    }
                    flame->SecondsSinceEmit = 0;
                }
            }
        }

        //Remove expired flames
        m_Flames.erase(std::remove_if(m_Flames.begin(), m_Flames.end(),
                                      [](std::shared_ptr<Flame> f) { return f->Expired; }), m_Flames.end());



    }
    void FireCreator::onImGuiRender(){
        ImGui::Begin("Fire");
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "Flame Particle Props (General)");
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "#Particles:  %d", m_ParticleSystem.getActiveParticleCount());
        ImGui::DragFloat3("Velocity", &m_BaseFlameProps.Velocity.x, 0.05f, 0.f);
        ImGui::DragFloat3("VelocityVariation", &m_BaseFlameProps.VelocityVariation.x, 0.05f, 0.f, 999.f);
        ImGui::DragFloat("GravityFactor", &m_BaseFlameProps.GravityFactor, 0.001f, 0.f, 999.f);
        ImGui::ColorEdit4("ColorBegin", &m_BaseFlameProps.ColorBegin.x);
        ImGui::ColorEdit4("ColorEnd", &m_BaseFlameProps.ColorEnd.x);
        ImGui::DragFloat("SizeBegin", &m_BaseFlameProps.SizeBegin, 0.01f, 0.f, 999.f);
        ImGui::DragFloat("SizeVariationFactor", &m_BaseFlameProps.SizeVariationFactor, 0.005f, 0.f, 1.f);
        ImGui::DragFloat("SizeEnd", &m_BaseFlameProps.SizeEnd, 0.01f, 0.f, 999.f);

        for (size_t i = 0; i < m_Flames.size(); ++i){
            ImGui::PushID(i);
            if (ImGui::TreeNode((std::string("Flame ") + std::to_string(i)).c_str())) {
                m_Flames[i]->OnImGuiRender();
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::End();
    }


    void FireCreator::startFlame(std::shared_ptr<Flame> flame){
        flame->Expiring = false;
        flame->Timer = 0.f;
        flame->Expired = false;
        flame->BuildingUp = true;
        m_Flames.emplace_back(flame);
    }

    void FireCreator::startExpiringFlameOfName(const std::string& name){
        for (auto& f : m_Flames){
            if (std::string(f->Name) == name){
                f->startExpiring();
            }
        }
    }
}