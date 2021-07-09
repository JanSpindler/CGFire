//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
using namespace particle;

namespace scene{
    class SmokeCreationEvent : public Event{
    public:
        explicit SmokeCreationEvent(SmokeCreator& smokeCreator, std::shared_ptr<SmokeStream> smokeStream)
                : m_SmokeCreator(smokeCreator),
                  m_SmokeStream(std::move(smokeStream)){}
        void OnAction() override{
            m_SmokeCreator.startSmokeStream(m_SmokeStream);
        }
        void OnImGuiRender(){
            m_SmokeStream->OnImGuiRender();
        }
    private:
        SmokeCreator& m_SmokeCreator;
        std::shared_ptr<SmokeStream> m_SmokeStream;
    };
}