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
                       float particleLifeTimeVariation)
            : Position(position),
              PositionVariation(positionVariation),
              WaterDirection(waterDirection),
              Speed(speed),
              SpeedVariationFactor(speedVariationFactor),
              ParticlesPerSecond(particlesPerSecond),
              BuildUpTime(buildUpTime),
              ExpiringTime(expiringTime),
              ParticleLifeTime(particleLifeTime),
              ParticleLifeTimeVariation(particleLifeTimeVariation) {
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
        ImGui::DragFloat3("Position", &Position.x, 0.05f);
        ImGui::DragFloat3("PositionVariation", &PositionVariation.x, 0.05f);
        ImGui::DragFloat3("WaterDirection", &WaterDirection.x, 0.01f);
        ImGui::DragFloat("Speed", &Speed, 0.1f, 0.f, 999.f);
        ImGui::DragFloat("SpeedVariation", &SpeedVariationFactor, 0.1f, 0.f, 999.f);
        ImGui::DragInt("ParticlesPerSecond", &ParticlesPerSecond, 1, 0, 999);
        ImGui::DragFloat("BuildUpTime", &BuildUpTime, 0.01f, 0.f, 10.f);
        ImGui::DragFloat("ExpiringTime", &ExpiringTime, 0.01f, 0.f, 10.f);
        ImGui::DragFloat("ParticleLifeTime", &ParticleLifeTime, 0.01f, 0.f, 999.f);
        ImGui::DragFloat("ParticleLifeTimeVariation", &ParticleLifeTimeVariation, 0.001f, 0.f, 999.f);
    }

    WaterCreator::WaterCreator(ParticleSystem &particleSystem)
            : m_ParticleSystem(particleSystem) {

        std::vector<std::string> waterTextures(8);
        for (size_t i = 0; i < 8; i++) {
            waterTextures[i] = DATA_ROOT + "water/watersprite" + std::to_string(i) + ".png";
        }
        // load textures
        for (auto &file : waterTextures) {
            m_Textures.emplace_back(std::make_shared<en::GLPictureTex>(
                    en::GLPictureTex::WrapMode::CLAMP_TO_BORDER,
                    en::GLPictureTex::FilterMode::NEAREST,
                    file, true));
        }


        m_ParticleSystem.InitializeTextures(m_Textures);

        m_BaseWaterJetProps.VelocityVariation = {0.f, 0.f, 0.f};
        m_BaseWaterJetProps.GravityFactor = 8.f;
        m_BaseWaterJetProps.ColorBegin = {255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f};
        m_BaseWaterJetProps.ColorEnd = {255 / 255.0f, 255 / 255.0f, 255 / 255.0f, 1.0f};
        m_BaseWaterJetProps.SizeBegin = 1.f;
        m_BaseWaterJetProps.SizeVariationFactor = 0.1f;
        m_BaseWaterJetProps.SizeEnd = 3.5f;
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
            } else if (waterJet->Expiring) {
                waterJet->Timer += ts;
                sizeFactor = 1.f - (waterJet->Timer / waterJet->ExpiringTime);
                if (waterJet->Timer > waterJet->ExpiringTime) {
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
        auto it = m_WaterJets.begin();
        while(it != m_WaterJets.end()){
            if ((*it)->Expired){
                it = m_WaterJets.erase(it);
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
        ImGui::DragFloat("SizeBegin", &m_BaseWaterJetProps.SizeBegin, 0.005f, 0.f, 999.f);
        ImGui::DragFloat("SizeVariationFactor", &m_BaseWaterJetProps.SizeVariationFactor, 0.005f, 0.f, 1.f);
        ImGui::DragFloat("SizeEnd", &m_BaseWaterJetProps.SizeEnd, 0.005f, 0.f, 999.f);

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
        m_WaterJets.emplace_back(waterJet);
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
                                                       const glm::vec3& relativePos, const glm::vec3& eulerAngles){

        auto w = GetWaterJetByName(waterJetName);
        if (w != nullptr){
            m_WaterJetToObjectConnections.emplace_back(w, obj, relativePos, eulerAngles);
        }
    }

    void WaterCreator::UpdateWaterJetToObjectConnections(){
        for (auto& c : m_WaterJetToObjectConnections){
            WaterJet* waterJet = std::get<0>(c);
            en::RenderObj* carry = std::get<1>(c);
            const glm::vec3& relativePos = std::get<2>(c);
            const glm::vec3& eulerAngles = std::get<3>(c);

            waterJet->Position = carry->Position + relativePos;


//            //TODO: Rotation
//            glm::quat qObject(carry->EulerAngles);
//            glm::quat qRelative = glm::angleAxis( glm::degrees(relativeRotAngle),
//                                                  glm::normalize(relativeRotAxis));
//
//            auto rot = qObject * qRelative;
//
//            glm::eulerAngles(rot);
//
//            glm::a
        }
    }
}