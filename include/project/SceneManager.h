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

#include "SceneEventsTypes.h"


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
                  m_SceneRenderer(1000, 800) //TODO

                  {

                      initObjects();
        }


        /**Initialization. Needs to be called when the scene (re-)starts*/
        void restart() {
            m_SceneRenderer.RemoveAllObjects();
            m_WaterCreator.clear();
            m_SmokeCreator.clear();
            m_FireCreator.clear();

            this->m_EventsAndTimesDone.clear();
            this->m_EventsAndTimes.clear();
            this->addEvents();
            m_EventsAndTimes.sort([](const auto& a, const auto& b) { return a.second < b.second; });

            m_SceneTime = 0.f;
            m_TimePaused = false;
        }

        void onUpdate(float deltaTime) {
            if (m_TimePaused)
                return;

            m_SceneTime += deltaTime;


            m_SceneRenderer.Resize(m_Window.GetWidth(), m_Window.GetHeight()); // TODO: maybe something more performant

            m_ParticleSystemWater.OnUpdate(deltaTime);
            m_ParticleSystemSmoke.OnUpdate(deltaTime);
            m_ParticleSystemFire.OnUpdate(deltaTime);
            m_WaterCreator.onUpdate(deltaTime);
            m_SmokeCreator.onUpdate(deltaTime);
            m_FireCreator.onUpdate(deltaTime);

            // Check if any event occurred, if so, delete it from the list (so it will only be called once)
            auto it = m_EventsAndTimes.begin();
            while (it != m_EventsAndTimes.end()){
                if (m_SceneTime >= (*it).second){
                    it->first->onAction(); // calls the event function

                    m_EventsAndTimesDone.emplace_back(*it);
                    it = m_EventsAndTimes.erase(it); //removes the event from the list
                }
                else
                    break;
            }
        }

        void OnRender(){
            m_SceneRenderer.Render(&m_Window, &m_Cam);
            m_ParticleSystemWater.OnRender();
            m_ParticleSystemSmoke.OnRender();
            m_ParticleSystemFire.OnRender();
        }

        void onImGuiRender() {
            ImGui::Begin("Menu");
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Time: %f", m_SceneTime);
            if (ImGui::Button("Restart")){
                this->restart();
            }
            if (ImGui::Button(m_TimePaused ? "Resume" : "Pause"))
                m_TimePaused = !m_TimePaused;
            ImGui::End();


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


        /*************Event related*/

        float m_SceneTime;
        bool m_TimePaused;

        //List of upcoming events with their respective times
        std::list<std::pair<std::shared_ptr<Event>, float>> m_EventsAndTimes;

        //List of done events with their respective times.
        // This is used because we dont want to delete the event objects by removing them from m_EventsAndTimes.
        // They could still be used. E.g. a smoke that has started expiring still is in the scene.
        std::list<std::pair<std::shared_ptr<Event>, float>> m_EventsAndTimesDone;


        void addEvent(const std::shared_ptr<Event>& event, float eventTime){
            m_EventsAndTimes.emplace_back(std::make_pair(event, eventTime));
        }

        //This is where the data of the events is coded, in file SceneEvents.cpp
        void addEvents();



        std::vector<std::pair<std::shared_ptr<en::Animator>, bool>> m_AnimatorsAnd;
        /***************Renderer*/
        en::SceneRenderer m_SceneRenderer;

        /***Objects that are always in the scene*/
        std::shared_ptr<en::DirLight> m_DirLight;
        std::shared_ptr<en::GLSkyboxTex> m_SkyboxTex;

        std::shared_ptr<en::Model> m_DummyPositionFinder;

        std::shared_ptr<en::Model> m_ModelFloor;

        std::shared_ptr<en::Model> m_ModelHouse;

//        std::shared_ptr<en::Model> m_ModelFireman;
//        std::shared_ptr<en::Animation> m_AnimationFireman;
//        std::shared_ptr<en::Animator> m_AnimatorFireman;

        std::shared_ptr<en::Model> m_ModelCar;


//        std::shared_ptr<en::Model> m_ReflectModel;

        std::shared_ptr<en::Spline3D> m_CamSpline;
        std::shared_ptr<en::Spline3DRenderable> m_CamSplineRenderable;

        //This is where all objects are loaded at the start of the program
        void initObjects();

    };
}