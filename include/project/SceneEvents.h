//
//Created by vincent on 24.06.2021.
// In this file all the event types are defined
#pragma once

#include <utility>

#include "particle/Fire.h"
#include "particle/Water.h"

namespace scene{

    //Abstract base class for events
    class Event{
    public:
        virtual void onAction() = 0;
    private:
    };

    using namespace particle;

    //Fire Creation
    class FireCreationEvent : public Event{
    public:
        explicit FireCreationEvent(FireCreator& fireCreator, std::shared_ptr<Flame> flame)
                : m_FireCreator(fireCreator),
                  m_Flame(std::move(flame)){}
        void onAction() override{
            m_FireCreator.startFlame(m_Flame);
        }
    private:
        FireCreator& m_FireCreator;
        std::shared_ptr<Flame> m_Flame;
    };


    //Water Creation
    class WaterCreationEvent : public Event{
    public:
        explicit WaterCreationEvent(WaterCreator& waterCreator, std::shared_ptr<WaterJet> waterJet)
                : m_WaterCreator(waterCreator),
                m_WaterJet(std::move(waterJet)){}
        void onAction() override{
            m_WaterCreator.startWaterJet(m_WaterJet);
        }
    private:
        WaterCreator& m_WaterCreator;
        std::shared_ptr<WaterJet> m_WaterJet;
    };

    //Fire Expiring
    class FireExpiringEvent : public Event{
    public:
        explicit FireExpiringEvent(std::shared_ptr<Flame> flame)
                : m_Flame(std::move(flame)){}
        void onAction() override{
            m_Flame->startExpiring();
        }
    private:
        std::shared_ptr<Flame> m_Flame;
    };

    //Water Expiring
    class WaterExpiringEvent : public Event{
    public:
        explicit WaterExpiringEvent(std::shared_ptr<WaterJet> waterJet)
                : m_WaterJet(std::move(waterJet)){}
        void onAction() override{
            m_WaterJet->startExpiring();
        }
    private:
        std::shared_ptr<WaterJet> m_WaterJet;
    };
}
