//
//Created by vincent on 24.06.2021.
//
#pragma once

#include <glm/gtx/transform.hpp>
#include <framework/imgui_util.hpp>
#include <engine/render/SceneRenderer.hpp>
#include <engine/model/Model.hpp>
#include <engine/prefab/SimplePointLight.hpp>
#include <RiggingAndBlending/animation.hpp>
#include <RiggingAndBlending/animator.hpp>

#include "particle/Water.h"
#include "particle/Smoke.h"
#include "particle/Fire.h"

#include "engine/Window.hpp"
#include "engine/Util.hpp"
#include "engine/input/Input.hpp"

#include "SceneEvent.h"
#include "EventManager.h"

#include "engine/Sceletal.hpp"

#include "ObjToSplineConnector.h"
#include "ObjectManager.h"
#include "EventManager.h"


namespace scene {
/**Organizes and manages all the events and objects of the scene.
 * This is supposed to be where we program the animation*/
    class SceneManager {
    public:
        explicit SceneManager(en::Camera &cam, en::Window& window)
                : m_Cam(cam),
                  m_Window(window),
                  m_ParticleSystemWater(3000, cam, false),
                  m_ParticleSystemSmoke(4000, cam, false),
                  m_ParticleSystemFire(3000, cam, true),
                  m_WaterCreator(m_ParticleSystemWater),
                  m_SmokeCreator(m_ParticleSystemSmoke),
                  m_FireCreator(m_ParticleSystemFire),
                  m_SceneRenderer(1000, 800),
                  m_EventManager(m_SceneRenderer, m_ObjectManager)
                  {

                      initObjects();
        }


        /**Initialization. Needs to be called when the scene (re-)starts*/
        void restart() {
            m_SceneRenderer.RemoveAllObjects();
            m_WaterCreator.clear();
            m_SmokeCreator.clear();
            m_FireCreator.clear();

            m_EventManager.OnResetTime();

            m_SceneTime = 0.f;
            m_TimePaused = false;
        }

        void onUpdate(float dt) {
            if (m_TimePaused)
                return;

            m_SceneTime += dt;

            m_SceneRenderer.Resize(m_Window.GetWidth(), m_Window.GetHeight()); // TODO: maybe something more performant

            m_ParticleSystemWater.OnUpdate(dt);
            m_ParticleSystemSmoke.OnUpdate(dt);
            m_ParticleSystemFire.OnUpdate(dt);
            m_WaterCreator.onUpdate(dt);
            m_SmokeCreator.onUpdate(dt);
            m_FireCreator.onUpdate(dt);

            m_EventManager.OnUpdate(m_SceneTime);

            m_SceneRenderer.Update(dt);
        }

        void OnRender(){
            m_SceneRenderer.Render(&m_Window, &m_Cam);
            m_ParticleSystemWater.OnRender();
            m_ParticleSystemSmoke.OnRender();
            m_ParticleSystemFire.OnRender();
            m_SceneRenderer.SetPrevViewMat(&m_Cam);
        }

        void onImGuiRender() {
            ImGui::Begin("Menu");
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Time: %f", m_SceneTime);
            if (ImGui::Button("Restart")){
                this->restart();
            }
            if (ImGui::Button(m_TimePaused ? "Resume" : "Pause"))
                m_TimePaused = !m_TimePaused;

            ImGui::DragFloat("Select Scene Time", &m_SceneTimeSelection, 0.1f, 0.f, 3600.f);
            ImGui::SameLine();
            if (ImGui::Button("Set Time to Selected")){
                this->restart();
                m_SceneTime = m_SceneTimeSelection;
            }

            if (ImGui::Button("Save Scene")){
                m_ObjectManager.SaveToFile();
                m_EventManager.SaveToFile();
            }

            ImGui::End();

            m_ObjectManager.OnImGuiRender();
            m_EventManager.OnImGuiRender();
            m_SceneRenderer.OnImGuiRender();
            m_WaterCreator.onImGuiRender();
            m_SmokeCreator.onImGuiRender();
            m_FireCreator.onImGuiRender();

        }

    private:
        en::Camera& m_Cam;
        en::Window& m_Window;

        /****************Particle Systems*/
        particle::ParticleSystem m_ParticleSystemWater;
        particle::ParticleSystem m_ParticleSystemSmoke;
        particle::ParticleSystem m_ParticleSystemFire;
        particle::WaterCreator m_WaterCreator;
        particle::SmokeCreator m_SmokeCreator;
        particle::FireCreator m_FireCreator;


        /****************Objects*/
        ObjectManager m_ObjectManager; //stores all objects of the scene that are static + object ui

        /*************Event related*/
        EventManager m_EventManager; //imports and exports Events + creation of event ui

        float m_SceneTime;
        bool m_TimePaused;
        float m_SceneTimeSelection = 0.f;



//        void createEvent(const std::shared_ptr<Event>& event, float eventTime){
//            m_EventsAndTimesSave.emplace_back(std::make_pair(event, eventTime));
//        }
//        //This is where the data of the events is coded, in file SceneEvents.cpp
//        void createEvents();



        std::vector<std::pair<std::shared_ptr<en::Animator>, bool>> m_AnimatorsAnd;

        /***************Renderer*/
        en::SceneRenderer m_SceneRenderer;

        /***Objects that are always in the scene*/
        std::shared_ptr<en::DirLight> m_DirLight;
        std::shared_ptr<en::GLSkyboxTex> m_SkyboxTex;

//        std::shared_ptr<en::Model> m_DummyPositionFinder;
//
//        std::shared_ptr<en::Model> m_ModelFloor;
//
//        std::shared_ptr<en::Model> m_ModelHouse;
//
//
//        std::shared_ptr<en::Sceletal> m_Vampire1;
//
//        std::shared_ptr<en::Model> m_ModelTruck;
//
//        std::shared_ptr<en::Spline3D> m_TruckSpline;
//        std::shared_ptr<en::Spline3DRenderable> m_TruckSplineRenderable;
//
//        std::shared_ptr<ObjToSplineConnector> m_TruckToTruckSplineConnector;
//
//
////        std::shared_ptr<en::Model> m_ReflectModel;
//
//        std::shared_ptr<en::Spline3D> m_CamSpline;
//        std::shared_ptr<en::Spline3DRenderable> m_CamSplineRenderable;

        //This is where all objects are loaded at the start of the program
        void initObjects();


    };
}