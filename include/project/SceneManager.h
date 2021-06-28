//
//Created by vincent on 24.06.2021.
//
#pragma once

#include <glm/gtx/transform.hpp>
#include <framework/imgui_util.hpp>

#include "particle/Water.h"
#include "particle/Smoke.h"
#include "particle/Fire.h"

#include "engine/Window.hpp"
#include "engine/Util.hpp"
#include "engine/input/Input.hpp"

#include "SceneEventsTypes.h"


namespace scene {
/**Organizes and manages all the objects of the scene.
 * This is supposed to be where we program the animation*/
    class SceneManager {
    public:
        explicit SceneManager(en::Camera &cam,
        particle::ParticleSystem &particleSystemWater,
                particle::ParticleSystem &particleSystemSmoke,
                particle::ParticleSystem &particleSystemFire)
                : m_Cam(cam),
                  m_WaterCreator(particleSystemWater),
                  m_SmokeCreator(particleSystemSmoke),
                  m_FireCreator(particleSystemFire) {
        }


        /**Initialization. Needs to be called when the scene (re-)starts*/
        void restart() {
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

        void onImGuiRender() {
            if (ImGui::Begin("Menu")) {
                ImGui::TextColored(ImVec4(0, 1, 0, 1), "%s", ("Time:" + std::to_string(m_SceneTime)).c_str());
                if (ImGui::Button("Restart")){
                    this->restart();
                }
                if (ImGui::Button(m_TimePaused ? "Resume" : "Pause"))
                    m_TimePaused = !m_TimePaused;


                ImGui::End();
            }

            m_WaterCreator.onImGuiRender();
            m_SmokeCreator.onImGuiRender();
            m_FireCreator.onImGuiRender();
        }

    private:
        en::Camera& m_Cam;
        particle::WaterCreator m_WaterCreator;
        particle::SmokeCreator m_SmokeCreator;
        particle::FireCreator m_FireCreator;

        float m_SceneTime;
        bool m_TimePaused;

        std::list<std::pair<std::shared_ptr<Event>, float>> m_EventsAndTimes; //List of upcoming events with their respective times

        //List of done events with their respective times.
        // This is used because we dont want to delete the event objects by removing them from m_EventsAndTimes.
        // They could still be used. E.g. a smoke that has started expiring still is in the scene.
        std::list<std::pair<std::shared_ptr<Event>, float>> m_EventsAndTimesDone;


        void addEvent(const std::shared_ptr<Event>& event, float eventTime){
            m_EventsAndTimes.emplace_back(std::make_pair(event, eventTime));
        }

        //This is where the data of the events is coded, in file SceneEvents.cpp
        void addEvents();

    };
}