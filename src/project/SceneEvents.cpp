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

        //Windows Front side global Coords:
        std::vector<glm::vec3> SmokeStreamsRightSide_Positions(13);
        SmokeStreamsRightSide_Positions[0] = glm::vec3(2.5f, 12.5f, -18.f);
        SmokeStreamsRightSide_Positions[1] = glm::vec3(1.7f, 12.5f, -18.f);


        std::vector<glm::vec3> splinePoints = {
                { 0.0f, 2.0f, 0.0f },
                { 5.0f, 5.0f, 0.0f },
                { 10.0f, 10.0f, -5.0f },
                { 35.0f, 30.0f, -15.0f }
        };




        auto spline = std::make_shared<en::Spline3D>(splinePoints, false, 40, en::Spline3D::TYPE_CATMULL_ROM);
        auto SmokeStream1 = std::make_shared<SmokeStream>
                                              (spline,
                                               glm::vec3(0.3f, 0.3f, 0.3f));

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

        m_DirLight = std::make_shared<en::DirLight>(glm::vec3(0.743f, -0.287f, -0.605f), glm::vec3(1.f));
        m_SceneRenderer.SetDirLight(m_DirLight.get());

        m_DummyPositionFinder = std::make_shared<en::Model>("cube.obj", true, "Positioning Dummy");
        m_DummyPositionFinder->Position = glm::vec3(0.0f, 0.0f, 0.0f);
        m_DummyPositionFinder->Scaling = glm::vec3(0.4f, 0.4f, 0.4f);

        m_ModelHouse = std::make_shared<en::Model>("scene/house/EdificioJoinedNoTex.obj", false, "House");
        m_ModelHouse->Position = glm::vec3(-10.f, -12.0f, -27.45f);
        m_ModelHouse->Scaling = glm::vec3(3.f, 3.f, 3.f);
        m_ModelHouse->RotationAngle = 4.675f;

        m_ModelFloor = std::make_shared<en::Model>("scene/ground/ground.obj", true, "floor");
        m_ModelFloor->Position = glm::vec3(0.0f, -25.0f, 0.0f);
        m_ModelFloor->Scaling = glm::vec3(1.0f, 1.0f, 1.0f);

//        m_ModelFireman = std::make_shared<en::Model>("scene/robot/toonbot/toon_bot.dae", false, "fireman");
//        m_ModelFireman->Position = glm::vec3(0.0f, 0.0f, 20.0f);
//        m_AnimationFireman = std::make_shared<en::Animation>("scene/robot/Robo.dae", m_ModelFireman.get());
//        m_AnimatorFireman = std::make_shared<en::Animator>(m_AnimationFireman.get());

        m_ModelCar = std::make_shared<en::Model>("scene/car/car_alt.obj", true, "Car");
        m_ModelCar->Position = glm::vec3(20.0f, 0.f, 20.0f);
        m_ModelCar->Scaling = glm::vec3(8.0f, 8.f, 8.0f);

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