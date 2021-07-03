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
        auto Flame1 = std::make_shared<Flame>(
                glm::vec3(0.5f, 0.5f, 0.5f),
                glm::vec3(1.f, 0.f, 1.f),
                30,
                15.f,
                10.f, 1.f,
                0.2f);


        this->addEvent(MakeSharedEvent(FireCreationEvent, (m_SceneRenderer, m_FireCreator, Flame1)),
                       5.f);

        this->addEvent(MakeSharedEvent(FireExpiringEvent, (m_SceneRenderer, Flame1)),
                       20.f);


        /********************WATER*/

        auto WaterJet1 = std::make_shared<WaterJet>(
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
        auto spline = std::make_shared<en::Spline3D>(splinePoints, false, 40, en::Spline3D::TYPE_NATURAL_CUBIC);
        auto SmokeStream1 = std::make_shared<SmokeStream>
                                              (spline,
                                               glm::vec3(0.5f, 0.5f, 0.5f),
                                               glm::vec3(1.f, 0.f, 1.f));

        this->addEvent(MakeSharedEvent(SmokeCreationEvent, (m_SmokeCreator, SmokeStream1)),
                       6.f);

        this->addEvent(MakeSharedEvent(SmokeExpiringEvent, (SmokeStream1)),
                       25.f);


        /******************MODELS*/

        this->addEvent(MakeSharedEvent(RenderObjCreationEvent, (m_SceneRenderer, m_DummyPositionFinder, RenderObjType::FixedColor)),
                       0.f);



        this->addEvent(MakeSharedEvent(RenderObjCreationEvent, (m_SceneRenderer, m_ModelFloor, RenderObjType::Standard)),
                       0.f);

        this->addEvent(MakeSharedEvent(RenderObjCreationEvent, (m_SceneRenderer, m_ModelHouse, RenderObjType::Standard)),
                       0.f);

//        this->addEvent(MakeSharedEvent(RenderObjCreationEvent, (m_SceneRenderer, m_ModelFireman, RenderObjType::Standard)),
//                       0.f);

        this->addEvent(MakeSharedEvent(RenderObjCreationEvent, (m_SceneRenderer, m_ModelCar, RenderObjType::Standard)),
                       0.f);

//        this->addEvent(MakeSharedEvent(RenderObjCreationEvent, (m_SceneRenderer, m_ReflectModel, RenderObjType::Reflective)),
//                       0.f);

        this->addEvent(MakeSharedEvent(RenderObjCreationEvent, (m_SceneRenderer, m_CamSplineRenderable, RenderObjType::Spline)),
                       0.f);


    }


    void SceneManager::initObjects(){

        m_SkyboxTex = std::make_shared<en::GLSkyboxTex>("CGFire/skybox1", ".png", false);
        m_SceneRenderer.SetSkyboxTex(m_SkyboxTex.get());

        m_DirLight = std::make_shared<en::DirLight>(glm::vec3(0.3f, -1.0f, 1.0f), glm::vec3(0.5f));
        m_SceneRenderer.SetDirLight(m_DirLight.get());

        m_DummyPositionFinder = std::make_shared<en::Model>("cube.obj", true, "Position Dummy");
        m_DummyPositionFinder->Position = glm::vec3(0.0f, 0.0f, 0.0f);

        m_ModelHouse = std::make_shared<en::Model>("scene/house/EdificioNoTrees.obj", true, "House");
        m_ModelHouse->Position = glm::vec3(40.0f, 0.0f, 40.0f);

        m_ModelFloor = std::make_shared<en::Model>("cube.obj", true, "floor");
        m_ModelFloor->Position = glm::vec3(0.0f, -2.0f, 0.0f);
        m_ModelFloor->Scaling = glm::vec3(10000.0f, 1.0f, 10000.0f);

//        m_ModelFireman = std::make_shared<en::Model>("scene/robot/toonbot/toon_bot.dae", false, "fireman");
//        m_ModelFireman->Position = glm::vec3(0.0f, 0.0f, 20.0f);
//        m_AnimationFireman = std::make_shared<en::Animation>("scene/robot/Robo.dae", m_ModelFireman.get());
//        m_AnimatorFireman = std::make_shared<en::Animator>(m_AnimationFireman.get());

        m_ModelCar = std::make_shared<en::Model>("scene/car/car.dae", true, "Car");
        m_ModelCar->Position = glm::vec3(20.0f, 0.f, 20.0f);

//        m_ReflectModel = std::make_shared<en::Model>("hd_sphere.obj", false, "ReflectSphere");
//        m_ReflectModel->Position = glm::vec3(0.0f, 0.0, -8.0f);


        std::vector<glm::vec3> splinePoints = {
                { 5.0f, 10.0f, 5.0f },
                { -5.0f, 0.0f, 5.0f },
                { -5.0f, 5.0f, -5.0f },
                { 5.0f, 0.0f, -5.0f }
        };
        m_CamSpline = std::make_shared<en::Spline3D>(splinePoints, false, 32, en::Spline3D::TYPE_CATMULL_ROM);
        m_CamSplineRenderable = std::make_shared<en::Spline3DRenderable>(m_CamSpline.get());

    }
}