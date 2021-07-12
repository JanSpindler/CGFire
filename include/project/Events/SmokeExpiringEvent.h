//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
using namespace particle;

namespace scene {
    class SmokeExpiringEvent : public Event {
    public:
        explicit SmokeExpiringEvent(std::shared_ptr <SmokeStream> smokeStream)
                : m_smokeStream(std::move(smokeStream)) {}

        void OnAction() override {
            m_smokeStream->startExpiring();
        }

        bool OnImGuiRender() override{
            m_smokeStream->OnImGuiRender();

            return true;
        }

    private:
        std::shared_ptr <SmokeStream> m_smokeStream;
    };
}