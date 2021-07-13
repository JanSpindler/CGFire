//
//Created by vincent on 13.07.2021.
//
#include "project/InitEvents.h"
#include "project/Events/ShowRenderObjEvent.h"

namespace scene {
    std::vector<std::unique_ptr<scene::Event>> DummyEventsOfAllTypes;

    void InitDummyEvents(en::SceneRenderer &sceneRenderer,
                         ObjectManager &objectManager) {
        DummyEventsOfAllTypes.clear();

        DummyEventsOfAllTypes.emplace_back(std::make_unique<scene::ShowRenderObjEvent>(sceneRenderer, objectManager));

    }
}