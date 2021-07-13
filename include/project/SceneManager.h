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
#include "EventInterface.h"

#include "engine/Sceletal.hpp"


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
                  m_SceneRenderer(1000, 800, true, false),
                  m_EventInterface(m_EventsAndTimesSave)
                  {

                      initObjects();
                      this->createEvents();
        }


        /**Initialization. Needs to be called when the scene (re-)starts*/
        void restart() {
            m_SceneRenderer.RemoveAllObjects();
            m_WaterCreator.clear();
            m_SmokeCreator.clear();
            m_FireCreator.clear();

            this->m_EventsAndTimesDone.clear();
            this->m_EventsAndTimes.clear();
            m_EventInterface.SortEvents();
            m_EventsAndTimes.assign(m_EventsAndTimesSave.begin(), m_EventsAndTimesSave.end());

            m_SceneTime = 0.f;
            m_TimePaused = false;
        }

        void onUpdate(float dt) {
            if (m_TimePaused)
                return;

            m_SceneTime += dt;


            m_SceneRenderer.Resize(m_Window.GetWidth(), m_Window.GetHeight());

            m_ParticleSystemWater.OnUpdate(dt);
            m_ParticleSystemSmoke.OnUpdate(dt);
            m_ParticleSystemFire.OnUpdate(dt);
            m_WaterCreator.onUpdate(dt);
            m_SmokeCreator.onUpdate(dt);
            m_FireCreator.onUpdate(dt);

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

            //m_SceneRenderer.Update(dt); // TODO
        }

        void OnRender(){
            m_SceneRenderer.Render(&m_Cam);
            m_ParticleSystemWater.OnRender();
            m_ParticleSystemSmoke.OnRender();
            m_ParticleSystemFire.OnRender();
            //m_SceneRenderer.SetPrevViewMat(&m_Cam); // TODO
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
            if (ImGui::Button("Set Time to Selected")){
                this->restart();
                m_SceneTime = m_SceneTimeSelection;
            }

            ImGui::End();

            m_EventInterface.OnImGuiRender();
            //m_SceneRenderer.OnImGuiRender(); // TODO
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
        float m_SceneTimeSelection = 0.f;


        //This is used to program some events in-game and export it to a file
        EventInterface m_EventInterface;


        //the original list of events.
        std::vector<std::pair<std::shared_ptr<Event>, float>> m_EventsAndTimesSave;

        //List of upcoming events with their respective times
        std::vector<std::pair<std::shared_ptr<Event>, float>> m_EventsAndTimes;

        //List of done events with their respective times.
        // This is used because we dont want to delete the event objects by removing them from m_EventsAndTimes.
        // They could still be used. E.g. a smoke that has started expiring still is in the scene.
        std::vector<std::pair<std::shared_ptr<Event>, float>> m_EventsAndTimesDone;


        void createEvent(const std::shared_ptr<Event>& event, float eventTime){
            m_EventsAndTimesSave.emplace_back(std::make_pair(event, eventTime));
        }
        //This is where the data of the events is coded, in file SceneEvents.cpp
        void createEvents();



        std::vector<std::pair<std::shared_ptr<en::Animator>, bool>> m_AnimatorsAnd;
        /***************Renderer*/
        en::SceneRenderer m_SceneRenderer;

        /***Objects that are always in the scene*/
        std::shared_ptr<en::DirLight> m_DirLight;
        std::shared_ptr<en::GLSkyboxTex> m_SkyboxTex;

        std::shared_ptr<en::Model> m_DummyPositionFinder;

        std::shared_ptr<en::Model> m_ModelFloor;

        std::shared_ptr<en::Model> m_ModelHouse;


        std::shared_ptr<en::Sceletal> m_Vampire1;

        std::shared_ptr<en::Model> m_ModelCar;


//        std::shared_ptr<en::Model> m_ReflectModel;

        std::shared_ptr<en::Spline3D> m_CamSpline;
        std::shared_ptr<en::Spline3DRenderable> m_CamSplineRenderable;

        //This is where all objects are loaded at the start of the program
        void initObjects();

    };
}