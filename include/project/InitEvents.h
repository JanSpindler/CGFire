//
//Created by vincent on 13.07.2021.
//

#pragma once

#include <vector>
#include <memory>

namespace en {
    class SceneRenderer;
}

namespace scene {
    class Event;

    class ObjectManager;

    /************************ATTENTION*************/
    /****Here add for each new event the type!*****/
    enum class EventType {
        ShowRenderObjEvent
    };

    //For creating new events, I use a list of dummy events. Each new event type must have a dummy event
    extern std::vector<std::unique_ptr<scene::Event>> DummyEventsOfAllTypes;

    /************************ATTENTION*************/
    /******We need to add for each event type one dummy event here (in .cpp)!******/
    void InitDummyEvents(en::SceneRenderer &sceneRenderer, ObjectManager &objectManager);
}