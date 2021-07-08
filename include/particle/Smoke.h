//
//Created by vincent on 27.06.2021.
// Creation of Smoke
#pragma once

#include "engine/render/GLTexture.hpp"
#include "particle/ParticleSystem.h"
#include <framework/imgui_util.hpp>
#include <engine/Spline3D.hpp>
#include <utility>
#include "engine/render/Light.hpp"

namespace particle{

    static int NumSmokeStreams = 0;
    /*A smoke stream can be placed in the scene, using a spline for the movement of the particles*/
    class SmokeStream {
        friend class SmokeCreator;
    public:
        explicit SmokeStream(
                std::shared_ptr<en::Spline3D> spline,
                const glm::vec3& positionVariation = glm::vec3(1.f, 0.f, 1.f),
                int particlesPerSecond = 14,
                float speed = 4.f,
                float speedVariation = 2.f,
                float buildUpTime = 10.f,
                float expiringTime = 20.f
                        )
        : Spline(std::move(spline)),
          PositionVariation(positionVariation),
          Speed(speed),
          SpeedVariation(speedVariation),
          ParticlesPerSecond(particlesPerSecond),
          BuildUpTime(buildUpTime),
          ExpiringTime(expiringTime)
        {
            ID = NumSmokeStreams++;
        }

        uint32_t ID;
        void startExpiring(){
            if (Timer < BuildUpTime)
                Timer = (1.f-(Timer/BuildUpTime))*ExpiringTime;
            else
                Timer = 0.f;
            BuildingUp = false; Expiring = true; }


        glm::vec3 PositionVariation;

        float Speed;
        float SpeedVariation;

        int ParticlesPerSecond;

        std::shared_ptr<en::Spline3D> Spline;


        void OnImGuiRender(){
            std::string strID = "SmokeStream" + std::to_string(ID);
            ImGui::PushID(strID.c_str());
            if (ImGui::TreeNode(strID.c_str())) {
                ImGui::DragFloat3("PositionVariation", &PositionVariation.x, 0.05f);
                ImGui::DragFloat("Speed", &Speed, 0.5f, 0.f, 999.f);
                ImGui::DragFloat("SpeedVariation", &SpeedVariation, 0.5f, 0.f, 999.f);
                ImGui::DragInt("ParticlesPerSecond", &ParticlesPerSecond, 1, 0, 999);
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


    /** This class manages all the smoke streams; it sends emit commands to the particle system*/
    class SmokeCreator{
    public:
        SmokeCreator(ParticleSystem& particleSystem);

        void onUpdate(float ts);
        void onImGuiRender();

        /** Adds a flame the list. Don't destroy Flames while FireCreator is alive!*/
        void startSmokeStream(std::shared_ptr<SmokeStream> smoke);

        void clear(){
            m_SmokeStreams.clear();
            m_ParticleSystem.clear();
        }
    private:

        ParticleSystem& m_ParticleSystem;
        std::vector<std::shared_ptr<en::GLPictureTex>> m_Textures; //the variety of textures we use for smoke
        ParticleProps m_BaseSmokeProps;

        std::vector<std::shared_ptr<SmokeStream>> m_SmokeStreams; //holds references to the smoke streams


    };
}