//
//Created by vincent on 10.07.2021.
//

//
//Created by vincent on 07.07.2021.
//

#include "project/EventManager.h"
#include "project/SceneManager.h"

namespace scene {
    EventManager::EventManager(SceneManager& sceneManager,
                               en::SceneRenderer &sceneRenderer,
                          scene::ObjectManager& objectManager)
            :
            m_SceneManager(sceneManager),
            m_SceneRenderer(sceneRenderer),
            m_ObjectManager(objectManager)
    {

        scene::InitDummyEvents(m_SceneRenderer, m_ObjectManager);




        //Safety copy
        std::filesystem::copy_file(SCENE_DATA_ROOT + SceneEventDataFileName,
                                   SCENE_DATA_ROOT + SceneEventDataAutoCopyBeforeStartFileName,
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
        ImGui::Begin("All Events");

        if (ImGui::Button("New Event")){
            ImGui::OpenPopup("Choose Event Type");
        }

        this->OnImGuiCreateEventRender();

        ImGui::SameLine();

        if (ImGui::Button("Sort")){
            SortEvents();
        }

        OnImGuiRenderEventsOfObj(nullptr); // nullptr means: render all events

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
                for (auto& eventTypeDummy : DummyEventsOfAllTypes)
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

            if (m_SelectedDummyEvent != nullptr){
                static float s_ChosenTime = 0.f;
                ImGui::DragFloat("Event Time", &s_ChosenTime, 0.005f, 0.f, 999.f);

                bool optionsAreOk = m_SelectedDummyEvent->OnImGuiRender();
                if (optionsAreOk){
                    if (ImGui::Button("OK")) {
                        //Create Event
                        auto s_EventToCreate = m_SelectedDummyEvent->Clone();

                        m_EventsAndTimes.emplace_back(std::make_pair(s_EventToCreate, s_ChosenTime));
                        s_EventToCreate->UpdateDescription();
                        SaveToFile();
                        m_SceneManager.restart(false);
                        ImGui::CloseCurrentPopup();
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                }

            }
            if (ImGui::Button("Abort")) {
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    scene::Event* EventManager::CreateEventFromType(scene::EventType type){
        for (auto& dummyEvent : DummyEventsOfAllTypes){
            if (dummyEvent->GetTypeID() == type){
                return dummyEvent->Clone();
            }
        }

        std::cout << "No such event type: " << static_cast<int>(type) << std::endl;
        return nullptr;
    }

    void EventManager::OnObjDeletion(en::RenderObj* obj){
        m_nextEventIndex = 0;
        //Delete all events related to obj
        auto it = m_EventsAndTimes.begin();
        while (it != m_EventsAndTimes.end()){
            if (it->first->IsRenderObjRelated(obj))
                it = m_EventsAndTimes.erase(it);
            else
                ++it;
        }
    }

    void EventManager::UpdateEventDescriptions(){
        for (auto p : m_EventsAndTimes){
            p.first->UpdateDescription();
        }
    }

    void EventManager::OnImGuiRenderEventsOfObj(en::RenderObj* obj){
        int deleteEvent = -1; //work-around to not have to delete while looping over the same vector
        for (int i = 0; i < m_EventsAndTimes.size(); i++){
            auto& e = m_EventsAndTimes[i];

            if (obj == nullptr || e.first->IsRenderObjRelated(obj)) {
                std::string EventTypeName = getEventTypeName(e.first);
                ImGui::PushID(e.first);

                ImGui::TextColored(ImVec4(0.5f, 1.f, 0.5f, 1.f), "%s",
                                   util::GetFloatPrecisionFixedString(e.second, 3).c_str());
                ImGui::SameLine();
                bool nodeOpen = ImGui::TreeNode(EventTypeName.c_str());

                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.5f, 1.f, 0.5f, 1.f), "%s", e.first->GetDescription().c_str());

                ImGui::SameLine();
                if (ImGui::Button("Clone Event")) {
                    //Clone Event
                    Event *clone = e.first->Clone();

                    m_EventsAndTimes.emplace_back(clone, e.second);
                    e = m_EventsAndTimes[i]; // emplace_back may reallocate the array, so e's address may change
                    clone->UpdateDescription();
                }

                ImGui::SameLine();
                if (ImGui::Button("Delete Event")) {
                    //Delete Event
                    deleteEvent = i;
                }

                if (nodeOpen) {
                    ImGui::PushItemWidth(100);
                    ImGui::DragFloat("Time", &e.second, 0.005f, 0.f, 999.f);
                    ImGui::PopItemWidth();

                    e.first->OnImGuiRender();
                    ImGui::TreePop();
                }

                ImGui::PopID();
            }
        }

        if (deleteEvent != -1){
            delete m_EventsAndTimes[deleteEvent].first;
            m_EventsAndTimes.erase(m_EventsAndTimes.begin() + deleteEvent);
        }
    }
    void EventManager::OnCreateNewObj(ObjectType type, en::RenderObj* obj){
        //Create new ShowRenderObjEvent as default for all new created objects so they appear immediately

        RenderObjType renderObjType = RenderObjType::Standard;

        if (type == ObjectType::Skeletal){
            renderObjType = RenderObjType::Skeletal;
        }else if (type == ObjectType::Spline){
            renderObjType = RenderObjType::FixedColor;
        }

        auto p = std::make_pair(new ShowRenderObjEvent(m_SceneRenderer, m_ObjectManager,
                                                   obj, renderObjType, true), 0.f);
        m_EventsAndTimes.emplace_back(p);
        p.first->UpdateDescription();
    }

}