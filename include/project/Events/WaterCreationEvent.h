//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
using namespace particle;

namespace scene{
    class WaterCreationEvent : public Event{
    public:
        explicit WaterCreationEvent(WaterCreator& waterCreator, std::shared_ptr<WaterJet> waterJet)
                : m_WaterCreator(waterCreator),
                  m_WaterJet(std::move(waterJet)){}
        void OnAction() override{
            m_WaterCreator.startWaterJet(m_WaterJet);
        }
        bool OnImGuiRender(){
            m_WaterJet->OnImGuiRender();

            return true;
        }
    private:
        WaterCreator& m_WaterCreator;
        std::shared_ptr<WaterJet> m_WaterJet;
    };
}