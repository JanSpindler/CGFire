//
//Created by vincent on 20.06.2021.
//

#include "particle/Water.h"
#include <glm/gtx/quaternion.hpp>


namespace particle {

    WaterJet::WaterJet(const char name[32],
                       const glm::vec3 &position,
                       const glm::vec3 &positionVariation,
                       const glm::vec3 &waterDirection,
                       float speed,
                       float speedVariationFactor,
                       int particlesPerSecond,
                       float buildUpTime,
                       float expiringTime,
                       float particleLifeTime,
                       float particleLifeTimeVariation,
                       float sizeBegin,
                       float sizeEnd)
            : Position(position),
              PositionVariation(positionVariation),
              WaterDirection(waterDirection),
              Speed(speed),
              SpeedVariationFactor(speedVariationFactor),
              ParticlesPerSecond(particlesPerSecond),
              BuildUpTime(buildUpTime),
              ExpiringTime(expiringTime),
              ParticleLifeTime(particleLifeTime),
              ParticleLifeTimeVariation(particleLifeTimeVariation),
              SizeBegin(sizeBegin),
              SizeEnd(sizeEnd){
        strcpy_s(Name, name);
    }


    void WaterJet::startExpiring() {
        if (Timer < BuildUpTime)
            Timer = (1.f - (Timer / BuildUpTime)) * ExpiringTime;
        else
            Timer = 0.f;
        BuildingUp = false;
        Expiring = true;
    }

    void WaterJet::OnImGuiRender() {
        ImGui::InputText("WaterJet Name", Name, IM_ARRAYSIZE(Name));
        ImGui::DragFloat3("Position", &Position.x, 0.01f);
        ImGui::DragFloat3("PositionVariation", &PositionVariation.x, 0.001f);
        ImGui::DragFloat3("WaterDirection", &WaterDirection.x, 0.01f);
        ImGui::DragFloat("Speed", &Speed, 0.1f, 0.f, 999.f);
        ImGui::DragFloat("SpeedVariation", &SpeedVariationFactor, 0.1f, 0.f, 999.f);
        ImGui::DragInt("ParticlesPerSecond", &ParticlesPerSecond, 1, 0, 999);
        ImGui::DragFloat("BuildUpTime", &BuildUpTime, 0.01f, 0.f, 10.f);
        ImGui::DragFloat("ExpiringTime", &ExpiringTime, 0.01f, 0.f, 10.f);
        ImGui::DragFloat("ParticleLifeTime", &ParticleLifeTime, 0.01f, 0.f, 999.f);
        ImGui::DragFloat("ParticleLifeTimeVariation", &ParticleLifeTimeVariation, 0.001f, 0.f, 999.f);
        ImGui::DragFloat("SizeBegin", &SizeBegin, 0.005f, 0.f, 999.f);
        ImGui::DragFloat("SizeEnd", &SizeEnd, 0.005f, 0.f, 999.f);
    }

    std::shared_ptr<WaterJet> WaterJet::LoadDataFromStrings(const std::vector<std::string>& data){
        std::string Name = data[0];
        glm::vec3 Position = glm::vec3(std::stof(data[1]), std::stof(data[2]), std::stof(data[3]));
        glm::vec3 PositionVariation = glm::vec3(std::stof(data[4]), std::stof(data[5]), std::stof(data[6]));
        glm::vec3 WaterDirection = glm::vec3(std::stof(data[7]), std::stof(data[8]), std::stof(data[9]));
        float Speed = std::stof(data[10]);
        float SpeedVariationFactor = std::stof(data[11]);
        int ParticlesPerSecond = std::stoi(data[12]);
        float BuildUpTime = std::stof(data[13]);
        float ExpireTime = std::stof(data[14]);
        float ParticleLifeTime = std::stof(data[15]);
        float ParticleLifeTimeVariation = std::stof(data[16]);
        float SizeBegin = std::stof(data[17]);
        float SizeEnd = std::stof(data[18]);


        auto waterJet = std::make_shared<WaterJet>(Name.c_str(), Position, PositionVariation, WaterDirection, Speed, SpeedVariationFactor,
                                            ParticlesPerSecond, BuildUpTime, ExpireTime, ParticleLifeTime, ParticleLifeTimeVariation,
                                            SizeBegin, SizeEnd);

        return waterJet;
    }

    void WaterJet::SaveSpecificDataToCSV(util::CSVWriter& csv) const{
        csv << Name
        << Position.x << Position.y << Position.z
        << PositionVariation.x << PositionVariation.y << PositionVariation.z
        << WaterDirection.x << WaterDirection.y << WaterDirection.z
        << Speed
        << SpeedVariationFactor
        << ParticlesPerSecond
        << BuildUpTime
        << ExpiringTime
        << ParticleLifeTime
        << ParticleLifeTimeVariation
        << SizeBegin
        << SizeEnd;
    }


    WaterCreator::WaterCreator(ParticleSystem &particleSystem, sound::SoundManager& soundManager)
            : m_ParticleSystem(particleSystem),
            m_SoundManager(soundManager){

        // TEXTURES
        std::vector<std::string> waterTextures(12);
        for (size_t i = 0; i < 12; i++) {
            waterTextures[i] = DATA_ROOT + "water/watersprite" + std::to_string(i) + ".png";
        }
        // load textures
        for (auto &file : waterTextures) {
            m_Textures.emplace_back(std::make_shared<en::GLPictureTex>(
                    en::GLPictureTex::WrapMode::CLAMP_TO_BORDER,
                    en::GLPictureTex::FilterMode::LINEAR,
                    file, true));
        }


        m_ParticleSystem.InitializeTextures(m_Textures);


        // AUDIO
        std::vector<std::string> audioFiles(1);
        for (size_t i = 0; i < 1; i++){
            audioFiles[i] = "water/waterjet" + std::to_string(i) + ".wav";
        }

        // get relevant buffers from sound manager
        for (auto& file : audioFiles){
            m_SoundBuffers.emplace_back(m_SoundManager.GetSoundBufferFromFile(file));
        }

        m_BaseWaterJetProps.VelocityVariation = {0.f, 0.f, 0.f};
        m_BaseWaterJetProps.GravityFactor = 8.f;
        m_BaseWaterJetProps.ColorBegin = {255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f};
        m_BaseWaterJetProps.ColorEnd = {255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f};
        m_BaseWaterJetProps.SizeVariationFactor = 0.1f;
        m_BaseWaterJetProps.TexCoordAnimFrames = {4, 4};

    }

    void WaterCreator::onUpdate(float ts) {
        //For all water jets emit new particles
        for (auto &waterJet : m_WaterJets) {
            float sizeFactor = 1.f; //will be set to less than 1 if the waterJet is building up or expiring
            if (waterJet->BuildingUp) {
                waterJet->Timer += ts;
                sizeFactor = waterJet->Timer / waterJet->BuildUpTime;
                if (waterJet->Timer > waterJet->BuildUpTime) {
                    waterJet->BuildingUp = false;
                    sizeFactor = 1.f;
                }
                waterJet->Sound.setVolume(sizeFactor * m_SndVolume);
            } else if (waterJet->Expiring) {
                waterJet->Timer += ts;
                sizeFactor = 1.f - (waterJet->Timer / waterJet->ExpiringTime);
                if (waterJet->Timer > waterJet->ExpiringTime) {
                    waterJet->Expired = true;
                    waterJet->Sound.stop();
                    sizeFactor = 1.f;
                }

                waterJet->Sound.setVolume(sizeFactor * m_SndVolume);
            }


            if (!waterJet->Expired) {
                waterJet->SecondsSinceEmit += ts;

                if (waterJet->ParticlesPerSecond > 0) {

                    const float FREQUENCY = 1.f / (sizeFactor * static_cast<float>(waterJet->ParticlesPerSecond));
                    if (waterJet->SecondsSinceEmit > FREQUENCY) {
                        auto numParticlesToEmit = static_cast<uint32_t>(waterJet->SecondsSinceEmit / FREQUENCY);
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
                            props.SizeBegin = waterJet->SizeBegin;
                            props.SizeEnd = waterJet->SizeEnd;

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
        auto it = m_WaterJets.begin();
        while(it != m_WaterJets.end()){
            if ((*it)->Expired){
                auto sp = *it;
                it = m_WaterJets.erase(it);
                sp.reset();
            }
            else
                ++it;
        }

        UpdateWaterJetToObjectConnections();
    }

    void WaterCreator::onImGuiRender() {
        ImGui::Begin("Water");
        ImGui::TextColored(ImVec4(0, 1, 1, 1), "WaterJet Particle Props (General)");
        ImGui::TextColored(ImVec4(1, 1, 1, 1), "#Particles:  %d", m_ParticleSystem.getActiveParticleCount());
        ImGui::DragFloat("GravityFactor", &m_BaseWaterJetProps.GravityFactor, 0.001f, 0.f, 999.f);
        ImGui::ColorEdit4("ColorBegin", &m_BaseWaterJetProps.ColorBegin.x);
        ImGui::ColorEdit4("ColorEnd", &m_BaseWaterJetProps.ColorEnd.x);
        ImGui::DragFloat("SizeVariationFactor", &m_BaseWaterJetProps.SizeVariationFactor, 0.005f, 0.f, 1.f);

        if (ImGui::DragFloat("SoundVolume", &m_SndVolume, 1.f, 0.f, 100.f)){
            for (auto& f : m_WaterJets){
                f->Sound.setVolume(m_SndVolume);
            }
        }
        if (ImGui::DragFloat("SoundAttenuation", &m_SndAttenuation, 0.1f, 0.f, 30.f)){
            for (auto& f : m_WaterJets){
                f->Sound.setAttenuation(m_SndAttenuation);
            }
        }
        if (ImGui::DragFloat("SoundMinDistance", &m_SndMinDistance, 0.1f, 0.f, 99999.f)){
            for (auto& f : m_WaterJets){
                f->Sound.setMinDistance(m_SndMinDistance);
            }
        }


        for (int i = 0; i < m_WaterJets.size(); ++i) {
            ImGui::PushID(i);
            if (ImGui::TreeNode((std::string("WaterJet ") + std::to_string(i)).c_str())) {
                m_WaterJets[i]->OnImGuiRender();
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::End();
    }

    void WaterCreator::startWaterJet(std::shared_ptr<WaterJet> waterJet) {
        waterJet->Expiring = false;
        waterJet->Timer = 0.f;
        waterJet->Expired = false;
        waterJet->BuildingUp = true;
        m_WaterJets.push_back(waterJet);

        auto& s = waterJet->Sound;
        s.setBuffer(*m_SoundBuffers[util::Random::Uint32(0, static_cast<uint32_t>(m_SoundBuffers.size())-1)]);

        s.setLoop(true);
        s.setPosition(waterJet->Position.x, waterJet->Position.y, waterJet->Position.z);
        s.setAttenuation(m_SndAttenuation);
        s.setMinDistance(m_SndMinDistance);
        s.play();
    }

    void WaterCreator::startExpiringWaterJetOfName(const std::string &name) {
        auto w = GetWaterJetByName(name);
        if (w != nullptr)
            w->startExpiring();
    }

    WaterJet* WaterCreator::GetWaterJetByName(const std::string& name){
        // This loop is ok for us. We don't use more than a couple Water Jets
        for (auto& w : m_WaterJets){
            if (std::string(w->Name) == name){
                return w.get();
            }
        }
        return nullptr;
    }

    void WaterCreator::ConnectWaterJetRelativeToObject(const std::string& waterJetName, en::RenderObj* obj,
                                                       const glm::vec3& relativePos, const glm::quat& quaternion){

        auto w = GetWaterJetByName(waterJetName);
        if (w != nullptr){
            for (auto& c : m_WaterJetToObjectConnections){
                auto carried = std::get<0>(c);
                if (std::string(carried->Name) == waterJetName){

                    //update
                    std::get<1>(c) = obj;
                    std::get<2>(c) = relativePos;
                    std::get<3>(c) = quaternion;

                    auto carry = std::get<1>(c);

                    carried->Position = carry->Position + carry->Quaternion * relativePos;
                    carried->WaterDirection = (carry->Quaternion * quaternion) * glm::vec3(0.f, 0.f, -1.f);

                    return; //already have it connected
                }
            }
            m_WaterJetToObjectConnections.emplace_back(w, obj, relativePos, quaternion);
        }
    }

    void WaterCreator::UpdateWaterJetToObjectConnections(){
        for (auto& c : m_WaterJetToObjectConnections){
            WaterJet* waterJet = std::get<0>(c);
            en::RenderObj* carry = std::get<1>(c);
            const glm::vec3& relativePos = std::get<2>(c);
            const glm::quat& relativeRot = std::get<3>(c);

            waterJet->Position = carry->Position + carry->Quaternion * relativePos;
            waterJet->WaterDirection = (carry->Quaternion * relativeRot) * glm::vec3(0.f, 0.f, -1.f);
        }
    }
}