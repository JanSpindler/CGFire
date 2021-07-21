
#include "project/SceneManager.h"

namespace scene {

    SceneManager::SceneManager(en::Camera &cam, en::Window& window)
            : m_Cam(cam),
              m_Window(window),
              m_ParticleSystemAdditiveBlendingOff(6000, cam, false),
              m_ParticleSystemAdditiveBlendingOn(3000, cam, true),
              m_SoundManager(m_Cam),
              m_WaterCreator(m_ParticleSystemAdditiveBlendingOff, m_SoundManager),
              m_SmokeCreator(m_ParticleSystemAdditiveBlendingOff),
              m_FireCreator(m_ParticleSystemAdditiveBlendingOn, m_SoundManager),
              m_SceneRenderer(1000, 800),
              m_ObjectManager(*this, m_EventManager),
              m_EventManager(m_Cam, *this, m_SceneRenderer, m_SoundManager, m_ObjectManager, m_FireCreator, m_WaterCreator, m_SmokeCreator)
    {

        initObjects();
    }

    void SceneManager::Restart(bool resetTime) {
        ReloadEvents();


        if (resetTime) {
            m_SceneTime = 0.f;
        }
        m_TimePaused = false;

    }

    void SceneManager::onUpdate(float dt) {
        if (m_AutoSave && m_TimeSinceAutoSave >= 5.f ){
            m_TimeSinceAutoSave = 0.f;
            Save();
        }
        if (m_ReloadEventsPeriodically && m_TimeSinceReloadEvents >= m_ReloadEventsEveryXSeconds){
            m_TimeSinceReloadEvents = 0.f;
            ReloadEvents();
        }


        m_FPS = static_cast<int>(1.f/dt);

        if (!m_TimePaused || m_TimePaused && m_DontPauseParticles) {
            m_ParticleSystemAdditiveBlendingOff.OnUpdate(dt);
            m_ParticleSystemAdditiveBlendingOn.OnUpdate(dt);
            m_WaterCreator.onUpdate(dt);
            m_SmokeCreator.onUpdate(dt);
            m_FireCreator.onUpdate(dt);
        }

        m_TimeSinceAutoSave += dt;
        m_TimeSinceReloadEvents += dt;

        if (m_TimePaused)
            return;

        m_SceneTime += dt;

        m_SceneRenderer.Resize(m_Window.GetWidth(), m_Window.GetHeight()); // TODO: maybe something more performant



        m_SoundManager.OnUpdate();

        m_ObjectManager.OnUpdate(dt);
        m_EventManager.OnUpdate(m_SceneTime);

        m_Cam.OnUpdate(dt);

        m_SceneRenderer.Update(dt);
    }

    void SceneManager::Save(const std::string& folderName){
        m_ObjectManager.SaveToFile(folderName);
        m_EventManager.SaveToFile(folderName);
    }

    void SceneManager::ReloadEvents(){
        m_SoundManager.OnResetTime();
        m_SceneRenderer.RemoveAllObjects();
        ClearParticles();

        m_ObjectManager.OnResetTime();
        m_EventManager.OnResetTime();

        m_SceneRenderer.AddStandardRenderObj(m_Terrain.get());
    }

    void SceneManager::OnRender(){
        m_SceneRenderer.Render(&m_Window, &m_Cam);
        m_ParticleSystemAdditiveBlendingOff.OnRender();
        m_ParticleSystemAdditiveBlendingOn.OnRender();
        m_SceneRenderer.SetPrevViewMat(&m_Cam);
    }

    void SceneManager::onImGuiRender() {
        ImGui::Begin("Menu");
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "Time: %f", m_SceneTime);
        ImGui::TextColored(ImVec4(0, 1, 0, 1), "FPS: %d", m_FPS);
        if (ImGui::Button("Restart")){
            this->Restart();
        }
        if (ImGui::Button(m_TimePaused ? "Resume" : "Pause"))
            m_TimePaused = !m_TimePaused;


        if (ImGui::Button("Set Time to:")){
            this->Restart();
            m_SceneTime = m_SceneTimeSelection;
        }
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragFloat("", &m_SceneTimeSelection, 0.1f, 0.f, 3600.f);
        ImGui::PopItemWidth();

        ImGui::Checkbox("Auto-Save every", &m_AutoSave);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragFloat("##AutoSaveEveryXSeconds:", &m_AutoSaveEveryXSeconds, 1.f, 0.f, 999.f);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::Text("seconds");
        ImGui::SameLine();
        if (ImGui::Button("Save Now")){
            this->Save();
        }
        ImGui::SameLine();

        static char s_Filename[32] = "";
        if (ImGui::Button("Save Scene as...")){
            ImGui::OpenPopup("Save Scene As");
            strcpy_s(s_Filename, "");
        }
        if (ImGui::BeginPopupModal("Save Scene As")){

            ImGui::InputText("Folder", s_Filename, IM_ARRAYSIZE(s_Filename));
            std::string saveAs_Name(s_Filename);
            if (!saveAs_Name.empty()){
                if (ImGui::Button("Save")) {
                    this->Save(saveAs_Name);
                    ImGui::CloseCurrentPopup();
                }
                ImGui::SameLine();
            }

            if (ImGui::Button("Abort")) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        ImGui::Checkbox("Auto-Reload Events every", &m_ReloadEventsPeriodically);
        ImGui::SameLine();
        ImGui::PushItemWidth(100);
        ImGui::DragFloat("##ReloadEventsEveryXSeconds", &m_ReloadEventsEveryXSeconds, 0.1f, 0.f, 999.f);
        ImGui::PopItemWidth();
        ImGui::SameLine();
        ImGui::Text("seconds");
        ImGui::SameLine();
        if (ImGui::Button("Reload Now")){
            this->ReloadEvents();
        }
        ImGui::SameLine();
        if (ImGui::Button("Clear Particles")){
            ClearParticles();
        }

        ImGui::Checkbox("Don't pause particles", &m_DontPauseParticles);

        ImGui::Checkbox("Show Splines", &m_ShowSplines);
        ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f),
                           "Make sure to turn these settings off\n"
                           "and press \"-\" to disable UI (\"+\" to enable), \n"
                           "if you want to play the scene correctly");

        ImGui::SameLine();
        ImGui::Text("Cam Position: %f/%f/%f\n ViewDirection: %f/%f/%f",
                    m_Cam.GetPos().x,m_Cam.GetPos().y, m_Cam.GetPos().z,
                    m_Cam.GetViewDir().x, m_Cam.GetViewDir().y, m_Cam.GetViewDir().z);

        m_SoundManager.OnImGuiRender();
        ImGui::End();

        m_ObjectManager.OnImGuiRender();
        m_EventManager.OnImGuiRender();
        m_SceneRenderer.OnImGuiRender();
        m_WaterCreator.onImGuiRender();
        m_SmokeCreator.onImGuiRender();
        m_FireCreator.onImGuiRender();

    }
    void SceneManager::ClearParticles(){
        m_WaterCreator.clear();
        m_SmokeCreator.clear();
        m_FireCreator.clear();
    }

    void SceneManager::initObjects(){

        m_SkyboxTex = std::make_shared<en::GLSkyboxTex>("CGFire/skybox1", ".png", false);
        m_SceneRenderer.SetSkyboxTex(m_SkyboxTex.get());

        m_DirLight = std::make_shared<en::DirLight>(glm::vec3(0.743f, -0.287f, -0.605f), glm::vec3(1.f));
        m_SceneRenderer.SetDirLight(m_DirLight.get());

        m_Terrain = std::make_shared<en::BackgroundTerrain>
                (100, 20.0f, -28.0f, 1.0f, 128.0f, 0.0f);
    }
}