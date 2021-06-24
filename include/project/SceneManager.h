//
//Created by vincent on 24.06.2021.
//
#pragma once

#include <glm/gtx/transform.hpp>
#include <framework/imgui_util.hpp>

#include "particle/Fire.h"
#include "particle/Water.h"

#include "engine/Window.hpp"
#include "engine/Util.hpp"
#include "engine/input/Input.hpp"

#include "SceneEvents.h"


namespace scene {
/**Organizes and manages all the objects of the scene.
 * This is supposed to be where we program the animation*/
    class SceneManager {
    public:
        explicit SceneManager(en::Camera &cam,
                              particle::FireCreator &fireCreator,
                              particle::WaterCreator &waterCreator)
                : m_Cam(cam),
                  m_FireCreator(fireCreator),
                  m_WaterCreator(waterCreator) {
        }


        /**Initialization. Needs to be called when the scene (re-)starts*/
        void restart() {
            m_FireCreator.clear();
            m_WaterCreator.clear();
            this->addEvents();
            m_SceneTime = 0.f;
        }

        void onUpdate(float deltaTime) {
            m_SceneTime += deltaTime;

            std::cout << m_SceneTime << std::endl;
            // Check if any event occurred, if so, delete it from the list (so it will only be called once)
            auto it = m_EventsAndTimes.begin();
            while (it != m_EventsAndTimes.end()){
                if (m_SceneTime >= (*it).second){
                    it->first->onAction(); // calls the event function

                    it = m_EventsAndTimes.erase(it); //removes the event from the list
                }
                else
                    ++it;
            }
        }

        void onImGuiRender() {
            if (ImGui::Begin("RestartButton")) {
                if (ImGui::Button("Restart")){
                    this->restart();
                }
                ImGui::End();
            }
        }

    private:
        en::Camera& m_Cam;
        particle::FireCreator& m_FireCreator;
        particle::WaterCreator& m_WaterCreator;

        float m_SceneTime;

        std::list<std::pair<std::shared_ptr<Event>, float>> m_EventsAndTimes; //List of events with their respective times

        void addEvent(const std::shared_ptr<Event>& event, float eventTime){
            m_EventsAndTimes.emplace_back(std::make_pair(event, eventTime));
        }

        //This is where the data of the events is coded, in file SceneEvents.cpp
        void addEvents();

    };
}