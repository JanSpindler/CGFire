//
//Created by vincent on 10.07.2021.
//

//
//Created by vincent on 07.07.2021.
//

#include "project/EventManager.h"

namespace scene {
    EventManager::EventManager(en::SceneRenderer &sceneRenderer,
                          scene::ObjectManager& objectManager)
            :
            m_SceneRenderer(sceneRenderer),
            m_ObjectManager(objectManager)
    {
        m_DummyEventsOfAllTypes.push_back(std::make_unique<scene::ShowRenderObjEvent>(m_SceneRenderer, m_ObjectManager));
        //... add for each event type one here



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

            scene::Event* ev = this->CreateEventFromType(type);

            if (ev == nullptr){
                continue;
            }

            ev->LoadDataFromStrings(row);

            m_EventsAndTimes.emplace_back(std::make_pair(ev, time));
        }
    }

    EventManager::~EventManager(){
        for (auto& e : m_EventsAndTimes){
            delete e.first;
        }
        m_EventsAndTimes.clear();
    }


    void EventManager::SaveToFile(){
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


    void EventManager::OnResetTime(){
        this->SortEvents();
        m_nextEventIndex = 0;
    }

    void EventManager::OnUpdate(float sceneTime){
        while (m_nextEventIndex < m_EventsAndTimes.size() && m_EventsAndTimes[m_nextEventIndex].second <= sceneTime){
            m_EventsAndTimes[m_nextEventIndex].first->OnAction(); // executes the event function
            m_nextEventIndex++;
        }
    }


    std::string getEventTypeName(scene::Event* e){
        std::string EventTypeName = typeid(*e).name();
        return EventTypeName.substr(strlen("class scene::"), EventTypeName.length());
    }

    void EventManager::OnImGuiRender(){
        ImGui::Begin("Events");

        if (ImGui::Button("New Event")){
            ImGui::OpenPopup("Choose Event Type");
        }

        this->OnImGuiCreateEventRender();

        if (ImGui::Button("Sort")){
            SortEvents();
        }

        for (int i = 0; i < m_EventsAndTimes.size(); i++){
            auto& e = m_EventsAndTimes[i];
            std::string EventTypeName = getEventTypeName(e.first);
            EventTypeName = std::to_string(i) + " " + EventTypeName;
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

    void EventManager::SortEvents(){
        std::sort(m_EventsAndTimes.begin(), m_EventsAndTimes.end(),
                  [](const auto& a, const auto& b) { return a.second < b.second; });
    }

    void EventManager::OnImGuiCreateEventRender(){
        if (ImGui::BeginPopupModal("Choose Event Type", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

            static std::string s_Selection;

            if (ImGui::BeginCombo("Event Type", s_Selection.c_str()))
            {
                for (auto& eventTypeDummy : m_DummyEventsOfAllTypes)
                {
                    std::string EventTypeName = getEventTypeName(eventTypeDummy.get());
                    bool is_selected = (s_Selection == EventTypeName);
                    if (ImGui::Selectable(EventTypeName.c_str(), is_selected)) {
                        s_Selection = EventTypeName;
                        m_SelectedDummyEvent = eventTypeDummy.get();
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            static Event* s_EventToCreate = nullptr;
            if (ImGui::Button("OK")) {

                bool isLegit = true;
                if (m_SelectedDummyEvent == nullptr)
                    isLegit = false;

                if (isLegit) {
                    //Create Event
                    s_EventToCreate = CreateEventFromType(m_SelectedDummyEvent->GetTypeID());
                    ImGui::OpenPopup("Set Event Options");

                } else {
                    ImGui::OpenPopup("Error");
                }
            }

            if (ImGui::BeginPopupModal("Error", nullptr)) {
                ImGui::Text("Something is wrong. Cannot create event.");
                if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            if (ImGui::BeginPopupModal("Set Event Options", nullptr)) {
                static float s_ChosenTime = 0.f;
                ImGui::DragFloat("Event Time", &s_ChosenTime, 0.005f, 0.f, 999.f);

                bool optionsAreOk = s_EventToCreate->OnImGuiRenderSetOptions();
                if (optionsAreOk){
                    if (ImGui::Button("OK")) {
                        m_EventsAndTimes.emplace_back(std::make_pair(s_EventToCreate, s_ChosenTime));

                        s_EventToCreate = nullptr;
                        ImGui::CloseCurrentPopup();
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                }
                if (ImGui::Button("Abort")) {
                    s_EventToCreate = nullptr;
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("Abort")) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    scene::Event* EventManager::CreateEventFromType(scene::EventType type){
        /**************************Here insert all event types //TODO make this somehow automatic (idk how. in python it would be easy)*/
        scene::Event* ev;
        if (type == EventType::ShowRenderObjEvent){
            ev = new scene::ShowRenderObjEvent(m_SceneRenderer, m_ObjectManager);
        } else {
            std::cout << "No such event type: " << static_cast<int>(type) << std::endl;
            ev = nullptr;
        }
        return ev;
    }
}