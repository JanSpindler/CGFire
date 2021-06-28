//
//Created by vincent on 24.06.2021.
//
#include "project/SceneManager.h"

namespace scene{
    //Here, add all of the events with their respective event time
    void SceneManager::addEvents(){
        using namespace particle;
#define MakeSharedEvent(x, y) std::static_pointer_cast<Event>(std::make_shared<x>y) //macro to simplify making of an event


        /********************FIRE*/
        std::shared_ptr<Flame> Flame1 = std::make_shared<Flame>(
                glm::vec3(0.5f, 0.5f, 0.5f),
                glm::vec3(1.f, 0.f, 1.f),
                30,
                15.f,
                10.f, 1.f,
                0.2f);


        this->addEvent(MakeSharedEvent(FireCreationEvent, (m_FireCreator, Flame1)),
                       5.f);

        this->addEvent(MakeSharedEvent(FireExpiringEvent, (Flame1)),
                       20.f);

//        this->addEvent(MakeSharedEvent(FireCreationEvent, (m_FireCreator, Flame1)),
//                       12.f);

        /********************WATER*/

        std::shared_ptr<WaterJet> WaterJet1 = std::make_shared<WaterJet>(
                glm::vec3(2.f, 0.f, 1.f),
                glm::vec3(0.5f, 0.5f, 0.5f),
                glm::vec3(1.f, 0.5f, 0.f),
                100.f,
                0.1f,
                500,
                10.f,
                2.f,
                5.f,
                0.2f);

        this->addEvent(MakeSharedEvent(WaterCreationEvent, (m_WaterCreator, WaterJet1)),
                       7.f);

        this->addEvent(MakeSharedEvent(WaterExpiringEvent, (WaterJet1)),
                       20.f);


        /********************SMOKE*/

        std::vector<glm::vec3> splinePoints = {
                { 0.0f, 2.0f, 0.0f },
                { 5.0f, 5.0f, 0.0f },
                { 10.0f, 10.0f, -5.0f },
                { 35.0f, 30.0f, -15.0f }
        };
        std::shared_ptr<en::Spline3D> spline = std::make_shared<en::Spline3D>(splinePoints, false, 40);
        auto SmokeStream1 = std::make_shared<SmokeStream>
                                              (spline,
                                               glm::vec3(0.5f, 0.5f, 0.5f),
                                               glm::vec3(1.f, 0.f, 1.f));

        this->addEvent(MakeSharedEvent(SmokeCreationEvent, (m_SmokeCreator, SmokeStream1)),
                       6.f);

        this->addEvent(MakeSharedEvent(SmokeExpiringEvent, (SmokeStream1)),
                       25.f);




    }
}