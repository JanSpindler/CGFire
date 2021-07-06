//
// Created by JS on 18/06/2021.
//

#include "engine/gr_include.hpp"
#include "engine/render/SceneRenderer.hpp"
#include "engine/Util.hpp"
#include "engine/config.hpp"
#include <glm/gtx/transform.hpp>

#include <framework/imgui_util.hpp>

namespace en
{
    SceneRenderer::SceneRenderer(int32_t width, int32_t height) :
            characterRenderObjs_({}),
            standardRenderObjs_({}),
            fixedColorRenderObjs_({}),
            dirLight_(nullptr),
            pointLights_({}),
            reflectiveMaps_({}),
            skyboxTex_(nullptr),
            gBuffer_(width, height)
    {
        LoadPrograms();
        CreateFullScreenVao();
        CreateSkyboxVao();
    }

    void SceneRenderer::Update(float deltaTime){
        for (auto* c : characterRenderObjs_) //update animation
            c->Update(deltaTime);
    }

    void SceneRenderer::Render(const Window* window, const Camera* cam) const
    {
        glm::mat4 viewMat = cam->GetViewMat();
        glm::mat4 skyboxViewMat = glm::mat4(glm::mat3(viewMat));
        glm::mat4 projMat = cam->GetProjMat();

        float* viewMatP = &viewMat[0][0];
        float* skyboxViewMatP = &skyboxViewMat[0][0];
        float* projMatP = &projMat[0][0];

        RenderDirShadow();
        RenderPointShadows();
        RenderReflectiveMaps();

        window->UseViewport();
        RenderDeferredGeometry(viewMatP, projMatP);
        RenderDeferredLighting(window, cam);

        gBuffer_.CopyDepthBufToDefaultFb();
        RenderFixedColor(viewMatP, projMatP);
        RenderSplines(viewMatP, projMatP);
        RenderReflectiveObj(cam->GetPos(), viewMatP, projMatP);

        RenderSkybox(skyboxViewMatP, projMatP);
    }

    void SceneRenderer::Resize(int32_t width, int32_t height)
    {
        gBuffer_.Resize(width, height);
    }

    void SceneRenderer::AddCharacterRenderObj(Character* renderObj)
    {
        RemoveCharacterRenderObj(renderObj);
        characterRenderObjs_.push_back(renderObj);
    }

    void SceneRenderer::RemoveCharacterRenderObj(const Character* renderObj)
    {
        for (uint32_t i = 0; i < characterRenderObjs_.size(); i++)
        {
            if (characterRenderObjs_[i] == renderObj)
            {
                characterRenderObjs_.erase(characterRenderObjs_.begin() + i);
                return;
            }
        }
    }

    void SceneRenderer::AddStandardRenderObj(RenderObj* renderObj)
    {
        RemoveStandardRenderObj(renderObj);
        standardRenderObjs_.push_back(renderObj);
    }

    void SceneRenderer::RemoveStandardRenderObj(const RenderObj* renderObj)
    {
        for (uint32_t i = 0; i < standardRenderObjs_.size(); i++)
        {
            if (standardRenderObjs_[i] == renderObj)
            {
                standardRenderObjs_.erase(standardRenderObjs_.begin() + i);
                return;
            }
        }
    }

    void SceneRenderer::AddFixedColorRenderObj(RenderObj* renderObj)
    {
        RemoveFixedColorRenderObj(renderObj);
        fixedColorRenderObjs_.push_back(renderObj);
    }

    void SceneRenderer::RemoveFixedColorRenderObj(const RenderObj* renderObj)
    {
        for (uint32_t i = 0; i < fixedColorRenderObjs_.size(); i++)
        {
            if (fixedColorRenderObjs_[i] == renderObj)
            {
                fixedColorRenderObjs_.erase(fixedColorRenderObjs_.begin() + i);
                return;
            }
        }
    }

    void SceneRenderer::AddSplineRenderObj(RenderObj* renderObj)
    {
        RemoveSplineRenderObj(renderObj);
        splineRenderObjs_.push_back(renderObj);
    }

    void SceneRenderer::RemoveSplineRenderObj(const RenderObj* renderObj)
    {
        for (uint32_t i = 0; i < splineRenderObjs_.size(); i++)
        {
            if (splineRenderObjs_[i] == renderObj)
            {
                splineRenderObjs_.erase(splineRenderObjs_.begin() + i);
                return;
            }
        }
    }

    void SceneRenderer::AddReflectiveRenderObj(RenderObj* renderObj)
    {
        RemoveReflectiveRenderObj(renderObj);
        reflectiveRenderObjs_.push_back(renderObj);
        reflectiveMaps_.push_back(ReflectiveMap(256));
    }

    void SceneRenderer::RemoveReflectiveRenderObj(const RenderObj* renderObj)
    {
        for (uint32_t i = 0; i < reflectiveRenderObjs_.size(); i++)
        {
            if (reflectiveRenderObjs_[i] == renderObj)
            {
                reflectiveRenderObjs_.erase(reflectiveRenderObjs_.begin() + i);
                reflectiveMaps_.erase(reflectiveMaps_.begin() + i);
                return;
            }
        }
    }

    void SceneRenderer::SetDirLight(DirLight *dirLight)
    {
        dirLight_ = dirLight;
    }

    void SceneRenderer::AddPointLight(const PointLight* pointLight)
    {
        RemovePointLight(pointLight);
        pointLights_.push_back(pointLight);
    }

    void SceneRenderer::RemovePointLight(const PointLight* pointLight)
    {
        for (uint32_t i = 0; i < pointLights_.size(); i++)
        {
            if (pointLights_[i] == pointLight)
            {
                pointLights_.erase(pointLights_.begin() + i);
                return;
            }
        }
    }

    void SceneRenderer::SetSkyboxTex(const GLSkyboxTex *skyboxTex)
    {
        skyboxTex_ = skyboxTex;
    }

    void SceneRenderer::RemoveAllObjects(){
        characterRenderObjs_.clear();
        standardRenderObjs_.clear();
        fixedColorRenderObjs_.clear();
        splineRenderObjs_.clear();
        reflectiveRenderObjs_.clear();
        reflectiveMaps_.clear();
        pointLights_.clear();
    }

    void SceneRenderer::OnImGuiRender(){
        ImGui::Begin("Objects");
        dirLight_->OnImGuiRender();

        const float MAXPOS = 120.f;


        for (int i = 0; i < characterRenderObjs_.size(); i++) {
            ImGui::PushID(("character" + std::to_string(i)).c_str());
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "character%d %s ", i, characterRenderObjs_[i]->GetName().c_str());
            ImGui::InputFloat3("Position", &characterRenderObjs_[i]->Position.x);
            ImGui::SliderFloat3("PositionSlider", &characterRenderObjs_[i]->Position.x, -MAXPOS, MAXPOS);
            ImGui::SliderFloat("RotationAngle", &characterRenderObjs_[i]->RotationAngle, -0, 6.28318530718f);
            ImGui::SliderFloat3("RotationAxisVector", &characterRenderObjs_[i]->RotationAxis.x, 0.f, 1.f);
            ImGui::InputFloat3("Scaling", &characterRenderObjs_[i]->Scaling.x);
            ImGui::PopID();
        }

        for (int i = 0; i < standardRenderObjs_.size(); i++) {
            ImGui::PushID(("standard" + std::to_string(i)).c_str());
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "standard%d %s ", i, standardRenderObjs_[i]->GetName().c_str());
            ImGui::InputFloat3("Position", &standardRenderObjs_[i]->Position.x);
            ImGui::SliderFloat3("PositionSlider", &standardRenderObjs_[i]->Position.x, -MAXPOS, MAXPOS);
            ImGui::SliderFloat("RotationAngle", &standardRenderObjs_[i]->RotationAngle, -0, 6.28318530718f);
            ImGui::SliderFloat3("RotationAxisVector", &standardRenderObjs_[i]->RotationAxis.x, 0.f, 1.f);
            ImGui::InputFloat3("Scaling", &standardRenderObjs_[i]->Scaling.x);
            ImGui::PopID();
        }

        for (int i = 0; i < fixedColorRenderObjs_.size(); i++) {
            ImGui::PushID(("fixedCol" + std::to_string(i)).c_str());
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "fixedCol%d %s ", i, fixedColorRenderObjs_[i]->GetName().c_str());
            ImGui::InputFloat3("Position", &fixedColorRenderObjs_[i]->Position.x);
            ImGui::SliderFloat3("PositionSlider", &fixedColorRenderObjs_[i]->Position.x, -MAXPOS, MAXPOS);
            ImGui::SliderFloat("RotationAngle", &fixedColorRenderObjs_[i]->RotationAngle, -0, 6.28318530718f);
            ImGui::SliderFloat3("RotationAxisVector", &fixedColorRenderObjs_[i]->RotationAxis.x, 0.f, 1.f);
            ImGui::InputFloat3("Scaling", &fixedColorRenderObjs_[i]->Scaling.x);
            ImGui::PopID();
        }

        for (int i = 0; i < splineRenderObjs_.size(); i++) {
            ImGui::PushID(("spline" + std::to_string(i)).c_str());
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "spline%d %s ", i, splineRenderObjs_[i]->GetName().c_str());
            ImGui::InputFloat3("Position", &splineRenderObjs_[i]->Position.x);
            ImGui::SliderFloat3("PositionSlider", &splineRenderObjs_[i]->Position.x, -MAXPOS, MAXPOS);
            ImGui::SliderFloat("RotationAngle", &splineRenderObjs_[i]->RotationAngle, -0, 6.28318530718f);
            ImGui::SliderFloat3("RotationAxisVector", &splineRenderObjs_[i]->RotationAxis.x, 0.f, 1.f);
            ImGui::InputFloat3("Scaling", &splineRenderObjs_[i]->Scaling.x);
            ImGui::PopID();
        }

        for (int i = 0; i < reflectiveRenderObjs_.size(); i++) {
            ImGui::PushID(("reflective" + std::to_string(i)).c_str());
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "reflective%d %s ", i, reflectiveRenderObjs_[i]->GetName().c_str());
            ImGui::InputFloat3("Position", &reflectiveRenderObjs_[i]->Position.x);
            ImGui::SliderFloat3("PositionSlider", &reflectiveRenderObjs_[i]->Position.x, -MAXPOS, MAXPOS);
            ImGui::SliderFloat("RotationAngle", &reflectiveRenderObjs_[i]->RotationAngle, -0, 6.28318530718f);
            ImGui::SliderFloat3("RotationAxisVector", &reflectiveRenderObjs_[i]->RotationAxis.x, 0.f, 1.f);
            ImGui::InputFloat3("Scaling", &reflectiveRenderObjs_[i]->Scaling.x);
            ImGui::PopID();
        }
        ImGui::End();
    }

    void SceneRenderer::LoadPrograms()
    {
        const GLShader* fixedColorVert = GLShader::Load("CGFire/fixed_color.vert");
        const GLShader* fixedColorFrag = GLShader::Load("CGFire/fixed_color.frag");
        fixedColorProgram_ = GLProgram::Load(fixedColorVert, nullptr, fixedColorFrag);

        const GLShader* dirShadowVert = GLShader::Load("CGFire/dir_shadow.vert");
        const GLShader* dirShadowFrag = GLShader::Load("CGFire/dir_shadow.frag");
        dirShadowProgram_ = GLProgram::Load(dirShadowVert, nullptr, dirShadowFrag);

        const GLShader* pointShadowVert = GLShader::Load("CGFire/point_shadow.vert");
        const GLShader* pointShadowGeom = GLShader::Load("CGFire/point_shadow.geom");
        const GLShader* pointShadowFrag = GLShader::Load("CGFire/point_shadow.frag");
        pointShadowProgram_ = GLProgram::Load(pointShadowVert, pointShadowGeom, pointShadowFrag);

        const GLShader* geomVert = GLShader::Load("CGFire/deferred_geometry.vert");
        const GLShader* geomFrag = GLShader::Load("CGFire/deferred_geometry.frag");
        geometryProgram_ = GLProgram::Load(geomVert, nullptr, geomFrag);

        const GLShader* sceletalVert = GLShader::Load("CGFire/sceletal.vert");
        const GLShader* sceletalFrag = GLShader::Load("CGFire/deferred_geometry.frag");
        characterProgram_ = GLProgram::Load(sceletalVert, nullptr, sceletalFrag);

        const GLShader* lightVert = GLShader::Load("CGFire/deferred_lighting.vert");
        const GLShader* lightFrag = GLShader::Load("CGFire/deferred_lighting.frag");
        lightingProgram_ = GLProgram::Load(lightVert, nullptr, lightFrag);

        const GLShader* skyboxVert = GLShader::Load("CGFire/skybox.vert");
        const GLShader* skyboxFrag = GLShader::Load("CGFire/skybox.frag");
        skyboxProgram_ = GLProgram::Load(skyboxVert, nullptr, skyboxFrag);

        const GLShader* reflectiveVert = GLShader::Load("CGFire/reflective.vert");
        const GLShader* reflectiveFrag = GLShader::Load("CGFire/reflective.frag");
        reflectiveProgram_ = GLProgram::Load(reflectiveVert, nullptr, reflectiveFrag);

        const GLShader* toEnvMapVert = GLShader::Load("CGFire/to_env_map.vert");
        const GLShader* toEnvMapFrag = GLShader::Load("CGFire/to_env_map.frag");
        toEnvMapProgram_ = GLProgram::Load(toEnvMapVert, nullptr, toEnvMapFrag);
    }

    void SceneRenderer::CreateFullScreenVao()
    {
        glm::vec3 vertices[] = {
                glm::vec3(-1.0f, -1.0f, 0.0f),
                glm::vec3(1.0f, -1.0f, 0.0f),
                glm::vec3(1.0f, 1.0f, 0.0f),
                glm::vec3(-1.0f, 1.0f, 0.0f)
        };

        uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

        glGenVertexArrays(1, &fullScreenVao_);
        glBindVertexArray(fullScreenVao_);

        uint32_t vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*) 0);

        uint32_t ibo;
        glGenBuffers(1, &ibo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void SceneRenderer::CreateSkyboxVao()
    {
        float skyboxVertices[] = {
                // positions
                -1.0f,  1.0f, -1.0f,
                -1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f, -1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,

                -1.0f, -1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f, -1.0f,  1.0f,
                -1.0f, -1.0f,  1.0f,

                -1.0f,  1.0f, -1.0f,
                1.0f,  1.0f, -1.0f,
                1.0f,  1.0f,  1.0f,
                1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f,  1.0f,
                -1.0f,  1.0f, -1.0f,

                -1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f, -1.0f,
                1.0f, -1.0f, -1.0f,
                -1.0f, -1.0f,  1.0f,
                1.0f, -1.0f,  1.0f
        };

        glGenVertexArrays(1, &skyboxVao_);
        glBindVertexArray(skyboxVao_);

        uint32_t vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        glBindVertexArray(0);
    }

    void SceneRenderer::RenderDirShadow() const
    {
        dirShadowProgram_->Use();
        glm::mat4 lightMat = dirLight_->GetLightMat();
        dirShadowProgram_->SetUniformMat4("light_mat", false, &lightMat[0][0]);

        dirLight_->BindShadowBuffer();
        for (RenderObj* renderObj : characterRenderObjs_)
            renderObj->RenderPosOnly(dirShadowProgram_);
        for (RenderObj* renderObj : standardRenderObjs_)
            renderObj->RenderPosOnly(dirShadowProgram_);
        for (RenderObj* renderObj : fixedColorRenderObjs_)
            renderObj->RenderPosOnly(dirShadowProgram_);
        for (RenderObj* renderObj : reflectiveRenderObjs_)
            renderObj->RenderPosOnly(dirShadowProgram_);
        dirLight_->UnbindShadowBuffer();
    }

    void SceneRenderer::RenderPointShadows() const
    {
        pointShadowProgram_->Use();
        std::vector<glm::mat4> lightMats;
        for (const PointLight* pointLight : pointLights_)
        {
            lightMats = pointLight->GetLightMats();
            for (uint32_t i = 0; i < 6; i++)
                pointShadowProgram_->SetUniformMat4("light_mats[" + std::to_string(i) + "]", false, &lightMats[i][0][0]);
            pointShadowProgram_->SetUniformVec3f("light_pos", pointLight->GetPos());

            pointLight->BindShadowBuffer();
            for (RenderObj* renderObj : characterRenderObjs_)
                renderObj->RenderPosOnly(pointShadowProgram_);
            for (RenderObj* renderObj : standardRenderObjs_)
                renderObj->RenderPosOnly(pointShadowProgram_);
            for (RenderObj* renderObj : fixedColorRenderObjs_)
                renderObj->RenderPosOnly(pointShadowProgram_);
            for (RenderObj* renderObj : reflectiveRenderObjs_)
                renderObj->RenderPosOnly(pointShadowProgram_);
            pointLight->UnbindShadowBuffer();
        }
    }

    void SceneRenderer::RenderDeferredGeometry(const float* viewMat, const float* projMat) const
    {
        gBuffer_.Bind();

        geometryProgram_->Use();
        geometryProgram_->SetUniformMat4("view_mat", false, viewMat);
        geometryProgram_->SetUniformMat4("proj_mat", false, projMat);
        for (RenderObj* renderObj : standardRenderObjs_)
            renderObj->RenderAll(geometryProgram_);

        characterProgram_->Use();
        characterProgram_->SetUniformMat4("view_mat", false, viewMat);
        characterProgram_->SetUniformMat4("proj_mat", false, projMat);
        for (RenderObj* renderObj : characterRenderObjs_)
            renderObj->RenderAll(characterProgram_);

        gBuffer_.Unbind();
    }

    void SceneRenderer::RenderDeferredLighting(const Window* window, const Camera* cam) const
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        lightingProgram_->Use();

        gBuffer_.UseTextures(lightingProgram_);
        lightingProgram_->SetUniformVec3f("cam_pos", cam->GetPos());

        glm::mat4 dirLightMat = dirLight_->GetLightMat();
        lightingProgram_->SetUniformMat4("dir_light_mat", false, &dirLightMat[0][0]);
        dirLight_->Use(lightingProgram_);
        dirLight_->UseShadow(lightingProgram_);

        const int32_t pointLightCount = std::min((int)pointLights_.size(), 24);
        lightingProgram_->SetUniformI("point_light_count", pointLightCount);
        for (uint32_t i = 0; i < pointLightCount; i++)
        {
            const PointLight* pointLight = pointLights_[i];
            pointLight->Use(lightingProgram_, i);
            pointLight->UseShadow(lightingProgram_, i);
        }

        glBindVertexArray(fullScreenVao_);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void SceneRenderer::RenderFixedColor(const float* viewMat, const float* projMat) const
    {
        fixedColorProgram_->Use();
        fixedColorProgram_->SetUniformMat4("view_mat", false, viewMat);
        fixedColorProgram_->SetUniformMat4("proj_mat", false, projMat);
        for (RenderObj* renderObj : fixedColorRenderObjs_)
            renderObj->RenderDiffuse(fixedColorProgram_);
    }

    void SceneRenderer::RenderSplines(const float *viewMat, const float *projMat) const
    {
        fixedColorProgram_->Use();
        fixedColorProgram_->SetUniformMat4("view_mat", false, viewMat);
        fixedColorProgram_->SetUniformMat4("proj_mat", false, projMat);
        for (RenderObj* renderObj : splineRenderObjs_)
            renderObj->RenderDiffuse(fixedColorProgram_);
    }

    void SceneRenderer::RenderReflectiveMaps() const
    {
        for (uint32_t i = 0; i < reflectiveMaps_.size(); i++)
        {
            const ReflectiveMap& reflectiveMap = reflectiveMaps_[i];
            RenderObj* reflectiveRenderObj = reflectiveRenderObjs_[i];

            uint32_t size = reflectiveMap.GetSize();
            glm::mat4 projMat = reflectiveMap.GetProjMat();
            std::vector<glm::mat4> viewMats = reflectiveMap.GetViewMats(reflectiveRenderObj->GetPos());
            float* projMatP = &projMat[0][0];

            reflectiveMap.BindFbo();
            glViewport(0, 0, size, size);

            reflectiveMap.BindCubeMap();

            toEnvMapProgram_->Use();
            toEnvMapProgram_->SetUniformMat4("proj_mat", false, projMatP);
            toEnvMapProgram_->SetUniformVec3f("obj_pos", reflectiveRenderObj->GetPos());
            for (uint32_t face = 0; face < 6; face++)
            {
                float* viewMatP = &viewMats[face][0][0];
                toEnvMapProgram_->SetUniformMat4("view_mat", false, viewMatP);

                reflectiveMap.BindCubeMapFace(face);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                for (RenderObj* renderObj : characterRenderObjs_)
                    renderObj->RenderDiffuse(toEnvMapProgram_);
                for (RenderObj* renderObj : standardRenderObjs_)
                    renderObj->RenderDiffuse(toEnvMapProgram_);
                for (RenderObj* renderObj : fixedColorRenderObjs_)
                    renderObj->RenderDiffuse(toEnvMapProgram_);
                for (RenderObj* renderObj : reflectiveRenderObjs_)
                {
                    if (renderObj != reflectiveRenderObj)
                        renderObj->RenderDiffuse(toEnvMapProgram_);
                }
                for (RenderObj* renderObj : splineRenderObjs_)
                    renderObj->RenderDiffuse(toEnvMapProgram_);
            }

            reflectiveMap.UnbindFbo();
        }
    }

    void SceneRenderer::RenderReflectiveObj(glm::vec3 camPos, const float *viewMat, const float *projMat) const
    {
        for (uint32_t i = 0; i < reflectiveRenderObjs_.size(); i++)
        {
            RenderObj* reflectiveRenderObj = reflectiveRenderObjs_[i];
            const ReflectiveMap& reflectiveMap = reflectiveMaps_[i];

            reflectiveProgram_->Use();
            reflectiveProgram_->SetUniformMat4("view_mat", false, viewMat);
            reflectiveProgram_->SetUniformMat4("proj_mat", false, projMat);
            reflectiveProgram_->SetUniformVec3f("cam_pos", camPos);

            glActiveTexture(GL_TEXTURE0);
            reflectiveMap.BindCubeMap();
            reflectiveProgram_->SetUniformI("skybox_tex", 0);
            reflectiveRenderObj->RenderDiffuse(reflectiveProgram_);
        }
    }

    void SceneRenderer::RenderSkybox(const float *viewMat, const float *projMat) const
    {
        if (skyboxTex_ == nullptr)
            return;

        glDepthFunc(GL_LEQUAL);

        skyboxProgram_->Use();
        skyboxProgram_->SetUniformMat4("view_mat", false, viewMat);
        skyboxProgram_->SetUniformMat4("proj_mat", false, projMat);

        glActiveTexture(GL_TEXTURE0);
        skyboxTex_->BindTex();
        skyboxProgram_->SetUniformI("skybox_tex", 0);

        glBindVertexArray(skyboxVao_);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);

        glDepthFunc(GL_LESS);
    }
}
