//
//Created by vincent on 07.07.2021.
//

#pragma once

#include "SceneEvent.h"
#include <framework/imgui_util.hpp>
#include <typeinfo>
#include <project/Events/ShowRenderObjEvent.h>

#include "util/CSVReader.h"
#include "ObjectManager.h"

namespace scene {

    //This class provides an ImGui interface for the events to manipulate them in game and save them to a file
    //and load them from a file
    class EventManager{
        const std::string SceneEventDataFileName = "sceneevents.csv";
        const std::string SceneEventDataAutoCopyFileName = "autocopy_sceneevents.csv";
        const std::string SceneEventDataAutoCopyBeforeStartFileName = "autocopylaststart_sceneevents.csv";
    public:
        explicit EventManager(en::SceneRenderer &sceneRenderer,
                              scene::ObjectManager& objectManager)
        :
          m_SceneRenderer(sceneRenderer),
          m_ObjectManager(objectManager),
          m_nextEvent(m_EventsAndTimes.begin())
        {
            //Safety copy
            std::filesystem::copy_file(SCENE_DATA_ROOT + SceneEventDataFileName, SCENE_DATA_ROOT + SceneEventDataAutoCopyBeforeStartFileName,
                                       std::filesystem::copy_options::overwrite_existing);


            //read scene events from file

            util::CSVReader csv(SCENE_DATA_ROOT + SceneEventDataFileName);
            auto& table = csv.getTable();

            for (int i = 0; i < table.size(); ++i) {
                auto row = table[i];

                std::string timeStr = row[0];
                if (timeStr.empty() || timeStr[0] == '#') { //don't care about empty lines or comments
                    continue;
                }

                //layout: time type <specific data>
                float time = std::stof(timeStr);
                EventType type = static_cast<EventType>(std::stoi(row[1]));

                row.erase(row.begin()); //erase time from string
                row.erase(row.begin()); //erase type from string

                scene::Event* ev;

                /**************************Here insert all event types //TODO make this somehow automatic*/
                if (type == EventType::ShowRenderObjEvent){
                    ev = new scene::ShowRenderObjEvent(m_SceneRenderer, m_ObjectManager);
                } else {
                    std::cout << "No such event type: " << static_cast<int>(type) << std::endl;
                    continue;
                }

                ev->LoadDataFromStrings(row);

                m_EventsAndTimes.emplace_back(std::make_pair(ev, time));
            }
        }

        ~EventManager(){
            for (auto& e : m_EventsAndTimes){
                delete e.first;
            }
            m_EventsAndTimes.clear();
        }


        void SaveToFile(){
            //first make a safe of the current scene before the loading. I
            // So, in case we made a mistake, we can still check out the old scene.
            std::filesystem::copy_file(SCENE_DATA_ROOT + SceneEventDataFileName, SCENE_DATA_ROOT + SceneEventDataAutoCopyFileName,
                                       std::filesystem::copy_options::overwrite_existing);

            //save the scene events

            using namespace util;
            CSVWriter csv(SCENE_DATA_ROOT + SceneEventDataFileName);

            this->SortEvents();

            //for each event, write in a row: EventTime,EventID,<EventSpecificData>
            for (auto& p : m_EventsAndTimes){
                Event* e = p.first;
                csv << p.second << static_cast<int>(e->GetTypeID());
                e->SaveSpecificDataToCSV(csv);
                csv << endrow;
            }

        }


        void OnResetTime(){
            this->SortEvents();
            m_nextEvent = m_EventsAndTimes.begin();
        }

        void OnUpdate(float sceneTime){
            while (m_nextEvent != m_EventsAndTimes.end() && m_nextEvent->second <= sceneTime){
                m_nextEvent->first->OnAction(); // executes the event function
                m_nextEvent++;
            }
        }

        void OnImGuiRender(){
            ImGui::Begin("Events");

            if (ImGui::Button("Sort")){
                SortEvents();
            }

            for (int i = 0; i < m_EventsAndTimes.size(); i++){
                auto& e = m_EventsAndTimes[i];
                std::string EventTypeName = typeid(*e.first).name();
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
        en::SceneRenderer& m_SceneRenderer;
        scene::ObjectManager& m_ObjectManager;
        size_t m_SelectedEventType = 0;


        std::vector<std::pair<scene::Event*, float>> m_EventsAndTimes;

        std::vector<std::pair<scene::Event*, float>>::iterator  m_nextEvent;

        void SortEvents(){
            std::sort(m_EventsAndTimes.begin(), m_EventsAndTimes.end(),
                      [](const auto& a, const auto& b) { return a.second < b.second; });
        }
    };
}