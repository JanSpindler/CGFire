//
//Created by vincent on 24.06.2021.
//
#pragma once

#include <glm/gtx/transform.hpp>
#include <framework/imgui_util.hpp>
#include <engine/render/SceneRenderer.hpp>
#include <engine/model/Model.hpp>
#include <engine/prefab/SimplePointLight.hpp>
#include <RiggingAndBlending/animation.hpp>
#include <RiggingAndBlending/animator.hpp>

#include "particle/Water.h"
#include "particle/Smoke.h"
#include "particle/Fire.h"

#include "engine/Window.hpp"
#include "engine/Util.hpp"
#include "engine/input/Input.hpp"

#include "SceneEvent.h"
#include "EventManager.h"

#include "engine/Skeletal.hpp"

#include "ObjectManager.h"
#include "EventManager.h"
#include <sound/SoundManager.h>
#include <engine/prefab/BackgroundTerrain.hpp>


namespace scene {
/**Organizes and manages all the events and objects of the scene.
 * This is supposed to be where we program the animation*/
    class SceneManager {
    public:
        explicit SceneManager(en::Camera &cam, en::Window& window);


        /**Initialization. Needs to be called when the scene (re-)starts*/
        void Restart(bool resetTime = true);

        void onUpdate(float dt);

        void OnRender();

        void onImGuiRender();

        bool AreSplinesDrawn(){ return m_ShowSplines; }
    private:
        en::Camera& m_Cam;
        en::Window& m_Window;

        /*************Sound**/
        sound::SoundManager m_SoundManager;

        /****************Particle Systems*/
        particle::ParticleSystem m_ParticleSystemAdditiveBlendingOff;
        particle::ParticleSystem m_ParticleSystemAdditiveBlendingOn;

        particle::WaterCreator m_WaterCreator;
        particle::SmokeCreator m_SmokeCreator;
        particle::FireCreator m_FireCreator;
        void ClearParticles();

        bool m_DontPauseParticles = false;

        /****************Objects*/
        ObjectManager m_ObjectManager; //stores all objects of the scene that are static + object ui

        /*************Event related*/
        EventManager m_EventManager; //imports and exports Events + creation of event ui

        //Scene time
        float m_SceneTime;
        bool m_TimePaused;
        float m_SceneTimeSelection = 0.f;

        //fps
        int m_FPS;

        //Auto Save
        bool m_AutoSave = true;
        float m_TimeSinceAutoSave = 0.f;
        float m_AutoSaveEveryXSeconds = 5.f;
        void Save(const std::string& folderName = "");

        //Auto reload Events--> Good for modelling the scene with actual data
        bool m_ReloadEventsPeriodically = false;
        float m_TimeSinceReloadEvents = 0.f;
        float m_ReloadEventsEveryXSeconds = 2.f;
        void ReloadEvents();


        bool m_ShowSplines = true;

        /***************Renderer*/
        en::SceneRenderer m_SceneRenderer;

        /***Objects that are always in the scene*/
        std::shared_ptr<en::DirLight> m_DirLight;
        std::shared_ptr<en::GLSkyboxTex> m_SkyboxTex;
        std::shared_ptr<en::BackgroundTerrain> m_Terrain;

        //This is where all objects are loaded at the start of the program
        void initObjects();

    };
}