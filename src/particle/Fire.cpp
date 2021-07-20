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
                 float particleLifeTimeVariation,
                 const glm::vec3& velocity,
                 const glm::vec3& velocityVariation,
                 float sizeBegin,
                 float sizeEnd)
            : PointLight(1.f),
              Position(position),
              PositionVariation(positionVariation),
              ParticlesPerEmit(particlesPerEmit),
              BuildUpTime(buildUpTime),
              ExpiringTime(expiringTime),
              ParticleLifeTime(particleLifeTime),
              ParticleLifeTimeVariation(particleLifeTimeVariation),
              Velocity(velocity),
              VelocityVariation(velocityVariation),
              SizeBegin(sizeBegin),
              SizeEnd(sizeEnd)

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
        if (ImGui::DragFloat3("Position", &Position.x, 0.001f)){
            Sound.setPosition(Position.x, Position.y, Position.z);
        }
        ImGui::DragFloat3("PositionVariation", &PositionVariation.x, 0.05f);
        ImGui::DragInt("ParticlesPerEmit", &ParticlesPerEmit, 1, 0, 999);
        ImGui::DragFloat("BuildUpTime", &BuildUpTime, 0.01f, 0.f, 999.f);
        ImGui::DragFloat("ExpiringTime", &ExpiringTime, 0.01f, 0.f, 999.f);
        ImGui::DragFloat("ParticleLifeTime", &ParticleLifeTime, 0.01f, 0.f, 999.f);
        ImGui::DragFloat("ParticleLifeTimeVariation", &ParticleLifeTimeVariation, 0.05f, 0.f, 999.f);
        ImGui::DragFloat3("Velocity", &Velocity.x, 0.005f, 0.f);
        ImGui::DragFloat3("VelocityVariation", &VelocityVariation.x, 0.001f, 0.f, 999.f);
        ImGui::DragFloat("SizeBegin", &SizeBegin, 0.001f, 0.f, 999.f);
        ImGui::DragFloat("SizeEnd", &SizeEnd, 0.001f, 0.f, 999.f);
    }

    std::shared_ptr<Flame> Flame::LoadDataFromStrings(const std::vector<std::string>& data){
        std::string Name = data[0];
        glm::vec3 Position = glm::vec3(std::stof(data[1]), std::stof(data[2]), std::stof(data[3]));
        glm::vec3 PositionVariation = glm::vec3(std::stof(data[4]), std::stof(data[5]), std::stof(data[6]));
        int ParticlesPerEmit = std::stoi(data[7]);
        float BuildUpTime = std::stof(data[8]);
        float ExpireTime = std::stof(data[9]);
        float ParticleLifeTime = std::stof(data[10]);
        float ParticleLifeTimeVariation = std::stof(data[11]);
        glm::vec3 Velocity = glm::vec3(std::stof(data[12]), std::stof(data[13]), std::stof(data[14]));
        glm::vec3 VelocityVariation = glm::vec3(std::stof(data[15]), std::stof(data[16]), std::stof(data[17]));
        float SizeBegin = std::stof(data[18]);
        float SizeEnd = std::stof(data[19]);


        auto flame = std::make_shared<Flame>(Name.c_str(), Position, PositionVariation, ParticlesPerEmit,
                                          BuildUpTime, ExpireTime, ParticleLifeTime, ParticleLifeTimeVariation,
                                          Velocity, VelocityVariation, SizeBegin, SizeEnd);
        return flame;
    }
    void Flame::SaveSpecificDataToCSV(util::CSVWriter& csv){
        csv << Name
            << Position.x << Position.y << Position.z
            << PositionVariation.x << PositionVariation.y << PositionVariation.z
            << ParticlesPerEmit
            << BuildUpTime
            << ExpiringTime
            << ParticleLifeTimeVariation
            << ParticleLifeTime
            << Velocity.x << Velocity.y << Velocity.z
            << VelocityVariation.x << VelocityVariation.y << VelocityVariation.z
            << SizeBegin << SizeEnd;
    }

    FireCreator::FireCreator(ParticleSystem& particleSystem, sound::SoundManager& soundManager)
            : m_ParticleSystem(particleSystem),
            m_SoundManager(soundManager)
    {

        // TEXTURES
        std::vector<std::string> sparkTextures(32);
        for (size_t i = 0; i < 32; i++){
            sparkTextures[i] = DATA_ROOT + "fire/fire(" + std::to_string(i) + ").png";
        }
        // load
        for (auto& file : sparkTextures){
            m_Textures.emplace_back(std::make_shared<en::GLPictureTex>(
                    en::GLPictureTex::WrapMode::CLAMP_TO_BORDER,
                    en::GLPictureTex::FilterMode::LINEAR,
                    file, true));
        }
        m_ParticleSystem.InitializeTextures(m_Textures);


        // AUDIO
        std::vector<std::string> audioFiles(3);
        for (size_t i = 0; i < 3; i++){
            audioFiles[i] = "fire/fire" + std::to_string(i) + ".wav";
        }

        // get relevant buffers from sound manager
        for (auto& file : audioFiles){
            m_SoundBuffers.emplace_back(m_SoundManager.GetSoundBufferFromFile(file));
        }



        m_BaseFlameProps.GravityFactor = 0.1f; // we barely want gravity to work on fire
        m_BaseFlameProps.ColorBegin = { 1.f, 1.f, 1.f, 1.0f };
        m_BaseFlameProps.ColorEnd = { 1.f, 1.f, 1.f, 0.3f };
        m_BaseFlameProps.SizeVariationFactor = 0.1f;
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
                flame->Sound.setVolume(flameSizeFactor * m_SndVolume);
            }
            else if (flame->Expiring){
                flame->Timer += ts;
                flameSizeFactor = 1.f - (flame->Timer / flame->ExpiringTime);
                if (flame->Timer > flame->ExpiringTime){
                    flame->Expired = true;
                    flameSizeFactor = 1.f;
                }
                flame->Sound.setVolume(flameSizeFactor * m_SndVolume);
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

                    props.Velocity = flame->Velocity;
                    props.VelocityVariation = flame->VelocityVariation;
                    props.SizeBegin = flameSizeFactor * flame->SizeBegin;
                    props.SizeEnd = flameSizeFactor * flame->SizeEnd;
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
        ImGui::DragFloat("GravityFactor", &m_BaseFlameProps.GravityFactor, 0.001f, 0.f, 999.f);
        ImGui::ColorEdit4("ColorBegin", &m_BaseFlameProps.ColorBegin.x);
        ImGui::ColorEdit4("ColorEnd", &m_BaseFlameProps.ColorEnd.x);
        ImGui::DragFloat("SizeVariationFactor", &m_BaseFlameProps.SizeVariationFactor, 0.005f, 0.f, 1.f);

        if (ImGui::DragFloat("SoundVolume", &m_SndVolume, 1.f, 0.f, 100.f)){
            for (auto& f : m_Flames){
                f->Sound.setVolume(m_SndVolume);
            }
        }
        if (ImGui::DragFloat("SoundAttenuation", &m_SndAttenuation, 0.1f, 0.f, 30.f)){
            for (auto& f : m_Flames){
                f->Sound.setAttenuation(m_SndAttenuation);
            }
        }
        if (ImGui::DragFloat("SoundMinDistance", &m_SndMinDistance, 0.1f, 0.f, 99999.f)){
            for (auto& f : m_Flames){
                f->Sound.setMinDistance(m_SndMinDistance);
            }
        }


        for (size_t i = 0; i < m_Flames.size(); ++i){
            ImGui::PushID(static_cast<int>(i));
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
        m_Flames.push_back(flame);

        auto& s = flame->Sound;
        s.setBuffer(*m_SoundBuffers[util::Random::Uint32(0, static_cast<uint32_t>(m_SoundBuffers.size())-1)]);

        s.setLoop(true);
        s.setPosition(flame->Position.x, flame->Position.y, flame->Position.z);
        s.setAttenuation(m_SndAttenuation);
        s.setMinDistance(m_SndMinDistance);
        s.play();
    }

    void FireCreator::startExpiringFlameOfName(const std::string& name){
        for (auto& f : m_Flames){
            if (std::string(f->Name) == name){
                f->startExpiring();
            }
        }
    }
}