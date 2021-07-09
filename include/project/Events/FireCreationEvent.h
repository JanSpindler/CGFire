//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
using namespace particle;

namespace scene {

    class FireCreationEvent : public Event {
    public:
        explicit FireCreationEvent(en::SceneRenderer &sceneRenderer, FireCreator &fireCreator,
                                   std::shared_ptr <Flame> flame)
                : m_SceneRenderer(sceneRenderer),
                  m_FireCreator(fireCreator),
                  m_Flame(std::move(flame)) {}

        void OnAction() override {
            m_FireCreator.startFlame(m_Flame);
            m_SceneRenderer.AddPointLight(m_Flame.get());
        }

        void OnImGuiRender() {
            m_Flame->OnImGuiRender();
        }

    private:
        en::SceneRenderer &m_SceneRenderer;
        FireCreator &m_FireCreator;
        std::shared_ptr <Flame> m_Flame;
    };


}