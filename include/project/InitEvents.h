//
//Created by vincent on 13.07.2021.
//

#pragma once

#include <vector>
#include <memory>

namespace en {
    class SceneRenderer;
    class Camera;
}
namespace particle {
    class FireCreator;
    class WaterCreator;
    class SmokeCreator;
}

namespace sound{
    class SoundManager;
}
namespace scene {
    class Event;

    class ObjectManager;


    /************************ATTENTION*************/
    /****Here add for each new event the type, AT THE BOTTOM of the list only!
     * Careful with changing EventType values*****/
    enum class EventType {
        ShowRenderObjEvent = 0,
        PlayAnimationEvent = 1,
        CreateFlameEvent = 2,
        ExpireFlameEvent = 3,
        CreateSmokeStreamEvent = 4,
        ExpireSmokeStreamEvent = 5,
        CreateWaterJetEvent = 6,
        ExpireWaterJetEvent = 7,
        ConnectCamToSplineEvent  = 8,
        ConnectObjectRelativeToObjectEvent  = 9,
        ConnectObjectToSplineEvent = 10,
        ConnectWaterRelativeToObjectEvent = 11,
        DisconnectObjectFromAllObjectsEvent = 12,
        SetObjectTransformEvent = 13,
        SetCamTransformEvent = 14,
        PlaySoundEffectEvent = 15,
        PlayMusicEvent = 16,

        SetCamFocusEvent = 17,
        SetObjectMotionBlurEvent = 18,
        PauseMusicEvent = 19,
        UnpauseMusicEvent = 20,
        RotateObjectOverTimeEvent = 21
    };

    //For creating new events, I use a list of dummy events. Each new event type must have a dummy event
    extern std::vector<std::unique_ptr<scene::Event>> DummyEventsOfAllTypes;

    /************************ATTENTION*************/
    /******We need to add for each event type one dummy event here (in .cpp)!******/
    void InitDummyEvents(en::Camera& cam,
                         en::SceneRenderer &sceneRenderer,
                         sound::SoundManager& soundManager,
                         ObjectManager &objectManager,
                         particle::FireCreator& fireCreator,
                         particle::WaterCreator& waterCreator,
                         particle::SmokeCreator& smokeCreator
                         );

    void DestroyDummyEvents();
}