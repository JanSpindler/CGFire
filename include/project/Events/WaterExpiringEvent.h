//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
using namespace particle;

namespace scene {
    class WaterExpiringEvent : public Event {
    public:
        explicit WaterExpiringEvent(std::shared_ptr <WaterJet> waterJet)
                : m_WaterJet(std::move(waterJet)) {}

        void OnAction() override {
            m_WaterJet->startExpiring();
        }

        bool OnImGuiRender() override{
            m_WaterJet->OnImGuiRender();

            return true;
        }

    private:
        std::shared_ptr <WaterJet> m_WaterJet;
    };
}