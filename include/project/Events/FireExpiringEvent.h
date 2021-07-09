//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
using namespace particle;

namespace scene{
    class FireExpiringEvent : public Event{
    public:
        explicit FireExpiringEvent(en::SceneRenderer& sceneRenderer, std::shared_ptr<Flame> flame)
                : m_SceneRenderer(sceneRenderer),
                  m_Flame(std::move(flame)){}
        void OnAction() override{
            m_Flame->startExpiring();
            m_SceneRenderer.RemovePointLight(m_Flame.get());
        }
        void OnImGuiRender(){
            m_Flame->OnImGuiRender();
        }
    private:
        en::SceneRenderer& m_SceneRenderer;
        std::shared_ptr<Flame> m_Flame;
    };
}