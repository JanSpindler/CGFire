//
//Created by vincent on 13.07.2021.
//

#include "project/InitEvents.h"

#include <particle/Fire.h>
#include <particle/Water.h>
#include <particle/Smoke.h>


#include <project/Events/ShowRenderObjEvent.h>
#include <project/Events/PlayAnimationEvent.h>
#include <project/Events/CreateFlameEvent.h>
#include <project/Events/CreateSmokeStreamEvent.h>
#include <project/Events/CreateWaterJetEvent.h>
#include <project/Events/ExpireFlameEvent.h>
#include <project/Events/ExpireWaterJetEvent.h>
#include <project/Events/ExpireSmokeStreamEvent.h>
#include <project/Events/ConnectCamToSplineEvent.h>
#include <project/Events/ConnectObjectRelativeToObjectEvent.h>
#include <project/Events/ConnectObjectToSplineEvent.h>
#include <project/Events/ConnectWaterRelativeToObjectEvent.h>
#include <project/Events/DisconnectObjectFromAllObjectsEvent.h>
#include <project/Events/SetObjectTransformEvent.h>
#include <project/Events/SetCamTransformEvent.h>
#include <project/Events/PlaySoundEffectEvent.h>
#include <project/Events/PlayMusicEvent.h>


namespace scene {
    std::vector<std::unique_ptr<scene::Event>> DummyEventsOfAllTypes;

    void InitDummyEvents(en::Camera& cam,
                         en::SceneRenderer &sceneRenderer,
                         sound::SoundManager& soundManager,
                         ObjectManager &objectManager,
                         particle::FireCreator& fireCreator,
                         particle::WaterCreator& waterCreator,
                         particle::SmokeCreator& smokeCreator
                         ) {
        DummyEventsOfAllTypes.clear();

        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::ShowRenderObjEvent>(sceneRenderer, objectManager));
        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::PlayAnimationEvent>(sceneRenderer, objectManager));

        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::CreateFlameEvent> (sceneRenderer, fireCreator));
        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::CreateSmokeStreamEvent> (sceneRenderer, smokeCreator, objectManager));
        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::CreateWaterJetEvent> (sceneRenderer, waterCreator));

        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::ExpireFlameEvent> (sceneRenderer, fireCreator));
        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::ExpireWaterJetEvent> (sceneRenderer, waterCreator));
        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::ExpireSmokeStreamEvent> (sceneRenderer, smokeCreator));

        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::ConnectObjectRelativeToObjectEvent>(objectManager));
        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::DisconnectObjectFromAllObjectsEvent>(objectManager));
        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::SetObjectTransformEvent>(objectManager));
        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::SetCamTransformEvent>(cam));
        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::ConnectCamToSplineEvent>(objectManager, cam));

        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::ConnectWaterRelativeToObjectEvent>(objectManager, waterCreator));
        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::PlaySoundEffectEvent>(soundManager, objectManager));
        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::PlayMusicEvent>(soundManager));
    }

    void DestroyDummyEvents(){
        DummyEventsOfAllTypes.clear();
    }
}