//
//Created by vincent on 13.07.2021.
//

#include "project/InitEvents.h"

#include <particle/Fire.h>
#include <particle/Fire.h>
#include <particle/Fire.h>


#include <project/Events/ShowRenderObjEvent.h>
#include <project/Events/PlayAnimationEvent.h>
#include <project/Events/CreateFlameEvent.h>
#include <project/Events/CreateSmokeStreamEvent.h>

namespace scene {
    std::vector<std::unique_ptr<scene::Event>> DummyEventsOfAllTypes;

    void InitDummyEvents(en::SceneRenderer &sceneRenderer,
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
    }
}