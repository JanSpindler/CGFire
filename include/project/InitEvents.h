//
//Created by vincent on 13.07.2021.
//

#pragma once

#include <vector>
#include <memory>

namespace en {
    class SceneRenderer;
}
namespace particle {
    class FireCreator;
    class WaterCreator;
    class SmokeCreator;
}

namespace scene {
    class Event;

    class ObjectManager;

    /************************ATTENTION*************/
    /****Here add for each new event the type, AT THE BOTTOM of the list only!*****/
    enum class EventType {
        ShowRenderObjEvent,
        PlayAnimationEvent,
        CreateFlameEvent,
        ExpireFlameEvent,
        CreateSmokeStreamEvent,
        ExpireSmokeStreamEvent,
        CreateWaterJetEvent,
        ExpireWaterJetEvent
    };

    //For creating new events, I use a list of dummy events. Each new event type must have a dummy event
    extern std::vector<std::unique_ptr<scene::Event>> DummyEventsOfAllTypes;

    /************************ATTENTION*************/
    /******We need to add for each event type one dummy event here (in .cpp)!******/
    void InitDummyEvents(en::SceneRenderer &sceneRenderer,
                         ObjectManager &objectManager,
                         particle::FireCreator& fireCreator,
                         particle::WaterCreator& waterCreator,
                         particle::SmokeCreator& smokeCreator
                         );
}