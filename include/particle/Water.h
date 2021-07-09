//
//Created by vincent on 20.06.2021.
// Calls the particle system to create a water
#pragma once

#include "engine/render/GLTexture.hpp"
#include "particle/ParticleSystem.h"
#include <framework/imgui_util.hpp>

namespace particle{

    static int NumWaterJets = 0;
    class WaterJet{
        friend class WaterCreator;
    public:

        explicit WaterJet(const glm::vec3& position,
                          const glm::vec3& positionVariation,
                          const glm::vec3& waterDirection,
                          float speed,
                          float speedVariationFactor,
                       int particlesPerSecond = 50,
                       float buildUpTime = 3.f,
                       float expiringTime = 1.f,
                       float particleLifeTime = 5.f,
                       float particleLifeTimeVariation = 0.2f)
                : Position(position),
                  PositionVariation(positionVariation),
                  WaterDirection(waterDirection),
                  Speed(speed),
                  SpeedVariationFactor(speedVariationFactor),
                  ParticlesPerSecond(particlesPerSecond),
                  BuildUpTime(buildUpTime),
                  ExpiringTime(expiringTime),
                  ParticleLifeTime(particleLifeTime),
                  ParticleLifeTimeVariation(particleLifeTimeVariation)
        {
            ID = NumWaterJets++;
        }


        void startExpiring(){
            if (Timer < BuildUpTime)
                Timer = (1.f-(Timer/BuildUpTime))*ExpiringTime;
            else
                Timer = 0.f;
            BuildingUp = false; Expiring = true; }

        uint32_t ID;
        glm::vec3 Position;
        glm::vec3 PositionVariation;
        glm::vec3 WaterDirection; // the direction of the water coming out of the source
        float Speed;
        float SpeedVariationFactor; //percentage-range of varying speed

        int ParticlesPerSecond;
        float ParticleLifeTime;
        float ParticleLifeTimeVariation;

        void OnImGuiRender(){
            std::string strID = "WaterJet" + std::to_string(ID);
            ImGui::PushID(strID.c_str());
            if (ImGui::TreeNode(strID.c_str())) {
                ImGui::DragFloat3("Position", &Position.x, 0.5f);
                ImGui::DragFloat3("PositionVariation", &PositionVariation.x, 0.05f);
                ImGui::DragFloat3("WaterDirection", &WaterDirection.x, 0.1f);
                ImGui::DragFloat("Speed", &Speed, 0.1f, 0.f, 999.f);
                ImGui::DragFloat("SpeedVariation", &SpeedVariationFactor, 0.1f, 0.f, 999.f);
                ImGui::DragInt("ParticlesPerSecond", &ParticlesPerSecond, 1, 0, 999);
                ImGui::DragFloat("ParticleLifeTime", &ParticleLifeTime, 0.1f, 0.f, 999.f);
                ImGui::DragFloat("ParticleLifeTimeVariation", &ParticleLifeTimeVariation, 0.05f, 0.f, 999.f);
                ImGui::TreePop();
            }
            ImGui::PopID();
        }

    private:
        const float BuildUpTime; //the amount of time the fire takes to come to its peak
        const float ExpiringTime; //the amount of time the fire takes to expire

        bool BuildingUp = true;
        bool Expiring = false; //if set true, the fire will start to expire
        bool Expired = false; //if set true, it is done completely

        float SecondsSinceEmit = 0.f; //internal emit timer

        float Timer = 0.f;

    };

    class WaterCreator{
    public:
        WaterCreator(ParticleSystem& particleSystem);

        void onUpdate(float ts);
        void onImGuiRender();

        /** Adds a water jet to the list. Don't destroy WaterJets while WaterCreator is alive!*/
        void startWaterJet(std::shared_ptr<WaterJet> waterJet);

        void clear(){
            m_WaterJets.clear();
            m_ParticleSystem.clear();
        }
    private:
        ParticleSystem& m_ParticleSystem;
        std::vector<std::shared_ptr<en::GLPictureTex>> m_Textures; //the variety of textures we use for water
        ParticleProps m_BaseWaterJetProps;
        std::vector<std::shared_ptr<WaterJet>> m_WaterJets; //references to the water jets

    };
}