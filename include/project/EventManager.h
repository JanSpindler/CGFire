//
//Created by vincent on 07.07.2021.
//

#pragma once

#include "InitEvents.h"
#include <framework/imgui_util.hpp>
#include <typeinfo>
#include <project/Events/ShowRenderObjEvent.h>
#include <engine/render/SceneRenderer.hpp>

#include "util/CSVReader.h"
#include "ObjectManager.h"

namespace scene {
    class SceneManager;
    class ObjectManager;
    enum class ObjectType;

    //This class provides an ImGui interface for the events to manipulate them in game and save them to a file
    //and load them from a file
    class EventManager{
        const std::string SceneEventDataFileName = "sceneevents.csv"; //this should always be the most recent edit
        const std::string SceneEventDataAutoCopyBeforeStartFileName = "autocopylaststart_sceneevents.csv";
    public:
        explicit EventManager(SceneManager& sceneManager,
                              en::SceneRenderer &sceneRenderer,
                              scene::ObjectManager& objectManager);

        ~EventManager();


        void SaveToFile();


        void OnResetTime();

        void OnUpdate(float sceneTime);

        void OnImGuiRender();

        void OnObjDeletion(en::RenderObj* obj);

        void UpdateEventDescriptions();

        void OnImGuiRenderEventsOfObj(en::RenderObj* obj);

        void OnCreateNewObj(ObjectType type, en::RenderObj* obj);
    private:
        SceneManager& m_SceneManager;
        en::SceneRenderer& m_SceneRenderer;
        scene::ObjectManager& m_ObjectManager;
        size_t m_SelectedEventType = 0;

        std::vector<std::pair<scene::Event*, float>> m_EventsAndTimes;
        size_t m_nextEventIndex = 0;


        scene::Event* m_SelectedDummyEvent = nullptr;

        void SortEvents();

        void OnImGuiCreateEventRender();

        scene::Event* CreateEventFromType(scene::EventType type);
    };
}