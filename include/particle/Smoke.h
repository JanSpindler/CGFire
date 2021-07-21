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
#include <util/CSVWriter.h>

namespace scene{
    class ObjectManager;
}
namespace particle{

    /*A smoke stream can be placed in the scene, using a spline for the movement of the particles*/
    class SmokeStream {
        friend class SmokeCreator;
    public:
        explicit SmokeStream(
                const char name[32] = "SmokeNoName",
                std::shared_ptr<en::Spline3D> spline = nullptr,
                const glm::vec3& positionVariation = glm::vec3(1.f, 0.f, 1.f),
                int particlesPerSecond = 14,
                float speed = 4.f,
                float speedVariation = 1.f,
                float sizeBegin = 3.f,
                float sizeEnd = 40.f,
                float sizeVariationFactor = 0.1f,
                float buildUpTime = 10.f,
                float expiringTime = 20.f);

        //Relevant Data
        char Name[32];
        std::shared_ptr<en::Spline3D> Spline;
        glm::vec3 PositionVariation;
        int ParticlesPerSecond;
        float Speed;
        float SpeedVariation;
        float SizeBegin;
        float SizeEnd;
        float SizeVariationFactor;
        float BuildUpTime; //the amount of time the fire takes to come to its peak
        float ExpiringTime; //the amount of time the fire takes to expire


        void startExpiring();
        void OnImGuiRender();
        static std::shared_ptr<SmokeStream> LoadDataFromStrings(scene::ObjectManager& objectManager,
                                                                const std::vector<std::string>& data);
        void SaveSpecificDataToCSV(util::CSVWriter& csv);
    private:
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

        void startExpiringSmokeStreamOfName(const std::string& name);

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