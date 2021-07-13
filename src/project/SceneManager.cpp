
#include "project/SceneManager.h"

namespace scene {

    SceneManager::SceneManager(en::Camera &cam, en::Window& window)
            : m_Cam(cam),
              m_Window(window),
              m_ParticleSystemWater(3000, cam, false),
              m_ParticleSystemSmoke(4000, cam, false),
              m_ParticleSystemFire(3000, cam, true),
              m_WaterCreator(m_ParticleSystemWater),
              m_SmokeCreator(m_ParticleSystemSmoke),
              m_FireCreator(m_ParticleSystemFire),
              m_SceneRenderer(1000, 800),
              m_ObjectManager(*this, m_EventManager),
              m_EventManager(*this, m_SceneRenderer, m_ObjectManager)
    {

        initObjects();
    }


    /**Initialization. Needs to be called when the scene (re-)starts*/
    void SceneManager::restart(bool resetTime) {
        m_SceneRenderer.RemoveAllObjects();
        m_WaterCreator.clear();
        m_SmokeCreator.clear();
        m_FireCreator.clear();

        m_EventManager.OnResetTime();

        if (resetTime)
            m_SceneTime = 0.f;
        m_TimePaused = false;
        m_AutoSave =  true;
        m_ReloadEventsPeriodically = true;
        m_RecalculateSplinesPeriodically = true;
    }

    void SceneManager::onUpdate(float dt) {
        if (m_AutoSave && m_TimeSinceAutoSave >= 5.f ){
            m_TimeSinceAutoSave = 0.f;
            m_ObjectManager.SaveToFile();
            m_EventManager.SaveToFile();
        }
        if (m_ReloadEventsPeriodically && m_TimeSinceReloadEvents >= m_ReloadEventsEveryXSeconds){
            m_TimeSinceReloadEvents = 0.f;
            this->restart(false);
        }

        if (m_RecalculateSplinesPeriodically){
            m_TimeSinceRecalculateSplines = 0.f;
            m_ObjectManager.OnRecalculateSplines();
        }

        m_FPS = static_cast<int>(1.f/dt);

        if (m_TimePaused)
            return;

        m_SceneTime += dt;
        m_TimeSinceAutoSave += dt;
        m_TimeSinceReloadEvents += dt;
        m_TimeSinceRecalculateSplines += dt;

        m_SceneRenderer.Resize(m_Window.GetWidth(), m_Window.GetHeight()); // TODO: maybe something more performant

        m_ParticleSystemWater.OnUpdate(dt);
        m_ParticleSystemSmoke.OnUpdate(dt);
        m_ParticleSystemFire.OnUpdate(dt);
        m_WaterCreator.onUpdate(dt);
        m_SmokeCreator.onUpdate(dt);
        m_FireCreator.onUpdate(dt);

        m_EventManager.OnUpdate(m_SceneTime);

        m_SceneRenderer.Update(dt);
    }

    void SceneManager::OnRender(){
        m_SceneRenderer.Render(&m_Window, &m_Cam);
        m_ParticleSystemWater.OnRender();
        m_ParticleSystemSmoke.OnRender();
        m_ParticleSystemFire.OnRender();
    }

    void SceneManager::onImGuiRender() {
        ImGui::Begin("Menu");
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Time: %f", m_SceneTime);
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "FPS: %d", m_FPS);
        if (ImGui::Button("Restart")){
            this->restart();
        }
        if (ImGui::Button(m_TimePaused ? "Resume" : "Pause"))
            m_TimePaused = !m_TimePaused;


        if (ImGui::Button("Set Time to:")){
            this->restart();
            m_SceneTime = m_SceneTimeSelection;
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragFloat("", &m_SceneTimeSelection, 0.1f, 0.f, 3600.f);
        ImGui::PopItemWidth();

        ImGui::Checkbox("Auto-Save every", &m_AutoSave);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragFloat("##AutoSaveEveryXSeconds:", &m_AutoSaveEveryXSeconds, 1.f, 0.f);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::Text("seconds");

        ImGui::Checkbox("Auto-Reload Events every", &m_ReloadEventsPeriodically);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragFloat("##ReloadEventsEveryXSeconds", &m_ReloadEventsEveryXSeconds, 0.1f, 0.f);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::Text("seconds");

        ImGui::Checkbox("Auto-Recalculate Splines every", &m_RecalculateSplinesPeriodically);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragFloat("##RecalculateSplinesEveryXSeconds", &m_RecalculateSplinesEveryXSeconds, 0.1f, 0.f);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::Text("seconds");

        ImGui::End();

        m_ObjectManager.OnImGuiRender();
        m_EventManager.OnImGuiRender();
        m_SceneRenderer.OnImGuiRender();
        m_WaterCreator.onImGuiRender();
        m_SmokeCreator.onImGuiRender();
        m_FireCreator.onImGuiRender();

    }
}