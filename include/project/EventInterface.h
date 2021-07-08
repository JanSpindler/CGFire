//
//Created by vincent on 07.07.2021.
//

#pragma once

#include "SceneEventsTypes.h"
#include <framework/imgui_util.hpp>
#include <typeinfo>

namespace scene {

    //This class provides an ImGui interface for the events to manipulate them in game and save them to a file
    //and load them from a file
    class EventInterface{
    public:
        explicit EventInterface(std::vector<std::pair<std::shared_ptr<Event>, float>>& EventsAndTimesSave)
        : m_EventsAndTimesSave(EventsAndTimesSave)
        {

        }
        void SortEvents(){
            std::sort(m_EventsAndTimesSave.begin(), m_EventsAndTimesSave.end(),
                      [](const auto& a, const auto& b) { return a.second < b.second; });
        }

        void OnImGuiRender(){
            ImGui::Begin("Events");

            if (ImGui::Button("Sort")){
                SortEvents();
            }


            for (int i = 0; i < m_EventsAndTimesSave.size(); i++){
                auto& e = m_EventsAndTimesSave[i];
                std::string EventTypeName = typeid(*e.first.get()).name();
                EventTypeName = std::to_string(i) + " " + EventTypeName.substr(strlen("class scene::"), EventTypeName.length());
                ImGui::PushID(EventTypeName.c_str());
                if (ImGui::TreeNode(EventTypeName.c_str())) {
                    ImGui::DragFloat("Time", &e.second, 0.005f, 0.f, 999.f);
                    e.first->OnImGuiRender();
                    ImGui::TreePop();
                }
                ImGui::PopID();
            }


            ImGui::End();

        }
    private:
        size_t m_SelectedEventType = 0;
        std::unordered_map<std::string, uint32_t> m_EventTypeNameToID;

        std::vector<std::pair<std::shared_ptr<Event>, float>>& m_EventsAndTimesSave;
    };
}