//
//Created by vincent on 24.06.2021.
// In this file all the event types are defined
#pragma once

#include <utility>

#include "particle/Water.h"
#include "particle/Smoke.h"
#include "particle/Fire.h"
#include <framework/imgui_util.hpp>

namespace scene{


    //Abstract base class for events
    class Event{
    public:
        virtual void onAction() = 0;
        virtual void OnImGuiRender() = 0;
    private:
    };

    using namespace particle;


    //Water Creation
    class WaterCreationEvent : public Event{
    public:
        explicit WaterCreationEvent(WaterCreator& waterCreator, std::shared_ptr<WaterJet> waterJet)
                : m_WaterCreator(waterCreator),
                  m_WaterJet(std::move(waterJet)){}
        void onAction() override{
            m_WaterCreator.startWaterJet(m_WaterJet);
        }
        void OnImGuiRender(){
            m_WaterJet->OnImGuiRender();
        }
    private:
        WaterCreator& m_WaterCreator;
        std::shared_ptr<WaterJet> m_WaterJet;
    };

    //Smoke Creation
    class SmokeCreationEvent : public Event{
    public:
        explicit SmokeCreationEvent(SmokeCreator& smokeCreator, std::shared_ptr<SmokeStream> smokeStream)
                : m_SmokeCreator(smokeCreator),
                  m_SmokeStream(std::move(smokeStream)){}
        void onAction() override{
            m_SmokeCreator.startSmokeStream(m_SmokeStream);
        }
        void OnImGuiRender(){
            m_SmokeStream->OnImGuiRender();
        }
    private:
        SmokeCreator& m_SmokeCreator;
        std::shared_ptr<SmokeStream> m_SmokeStream;
    };

    //Fire Creation
    class FireCreationEvent : public Event{
    public:
        explicit FireCreationEvent(en::SceneRenderer& sceneRenderer, FireCreator& fireCreator, std::shared_ptr<Flame> flame)
                :   m_SceneRenderer(sceneRenderer),
                    m_FireCreator(fireCreator),
                    m_Flame(std::move(flame)){}
        void onAction() override{
            m_FireCreator.startFlame(m_Flame);
            m_SceneRenderer.AddPointLight(m_Flame.get());
        }
        void OnImGuiRender(){
            m_Flame->OnImGuiRender();
        }
    private:
        en::SceneRenderer& m_SceneRenderer;
        FireCreator& m_FireCreator;
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
        void OnImGuiRender(){
            m_WaterJet->OnImGuiRender();
        }
    private:
        std::shared_ptr<WaterJet> m_WaterJet;
    };


    //Smoke Expiring
    class SmokeExpiringEvent : public Event{
    public:
        explicit SmokeExpiringEvent(std::shared_ptr<SmokeStream> smokeStream)
                : m_smokeStream(std::move(smokeStream)){}
        void onAction() override{
            m_smokeStream->startExpiring();
        }
        void OnImGuiRender(){
            m_smokeStream->OnImGuiRender();
        }
    private:
        std::shared_ptr<SmokeStream> m_smokeStream;
    };

    //Fire Expiring
    class FireExpiringEvent : public Event{
    public:
        explicit FireExpiringEvent(en::SceneRenderer& sceneRenderer, std::shared_ptr<Flame> flame)
                : m_SceneRenderer(sceneRenderer),
                m_Flame(std::move(flame)){}
        void onAction() override{
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



    enum RenderObjType {Char, Standard, FixedColor, Spline, Reflective };

    //RenderObj Creation
    class RenderObjCreationEvent : public Event{
    public:
        explicit RenderObjCreationEvent(en::SceneRenderer& sceneRenderer, std::shared_ptr<en::RenderObj> obj, RenderObjType type)
                :   m_SceneRenderer(sceneRenderer),
                    m_Obj(std::move(obj)),
                    m_Type(type)
                    {}
        void onAction() override{
            switch (m_Type){
                case Char:
                    m_SceneRenderer.AddSkeletalRenderObj(m_Obj.get());
                    break;
                case Standard:
                    m_SceneRenderer.AddStandardRenderObj(m_Obj.get());
                    break;
                case FixedColor:
                    m_SceneRenderer.AddFixedColorRenderObj(m_Obj.get());
                    break;
                case Spline:
                    m_SceneRenderer.AddSplineRenderObj(m_Obj.get());
                    break;
                case Reflective:
                    m_SceneRenderer.AddReflectiveRenderObj(m_Obj.get(), 2.0f, 0.5f);
                    break;
            }
        }
        void OnImGuiRender(){
            m_Obj->OnImGuiRender();
        }
    private:
        en::SceneRenderer& m_SceneRenderer;
        std::shared_ptr<en::RenderObj> m_Obj;
        RenderObjType m_Type;
    };


}
