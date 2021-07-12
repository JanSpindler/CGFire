//
// Created by JS on 18/06/2021.
//

#include "engine/gr_include.hpp"
#include "engine/render/SceneRenderer.hpp"
#include "engine/Util.hpp"
#include "engine/config.hpp"
#include <glm/gtc/random.hpp>

namespace en
{
    SceneRenderer::SceneRenderer(uint32_t width, uint32_t height, bool advancedShadow, bool postProcess) :
            advancedShadow_(advancedShadow),
            postProcess_(postProcess),
            standardRenderObjs_({}),
            fixedColorRenderObjs_({}),
            dirLight_(nullptr),
            pointLights_({}),
            reflectiveMaps_({}),
            skyboxTex_(nullptr),
            width_(width),
            height_(height),
            gBuffer_(width, height)
    {
        LoadPrograms();
        CreateFullScreenVao();
        CreateSkyboxVao();
        CreateScreenTmpTex();
    }

    void SceneRenderer::Render(const Camera* cam) const
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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glViewport(0, 0, width_, height_);

        RenderDeferredGeometry(viewMatP, projMatP);
        RenderDeferredLighting(cam->GetPos());

        gBuffer_.CopyDepthBufToDefaultFb();
        RenderFixedColor(viewMatP, projMatP);
        RenderSplines(viewMatP, projMatP);
        RenderReflectiveObj(cam->GetPos(), viewMatP, projMatP);

        RenderSkybox(skyboxViewMatP, projMatP);

        if (postProcess_)
            PostProcess(width_, height_);
    }

    void SceneRenderer::Resize(uint32_t width, uint32_t height)
    {
        if (width == 0 || height == 0)
            return;
        if (width == width_ && height == height_)
            return;

        gBuffer_.Resize(width, height);

        glBindTexture(GL_TEXTURE_2D, screenTmpTex0_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);

        glBindTexture(GL_TEXTURE_2D, screenTmpTex1_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, 0);

        glBindTexture(GL_TEXTURE_2D, 0);

        width_ = width;
        height_ = height;
    }

    void SceneRenderer::AddStandardRenderObj(const RenderObj* renderObj)
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

    void SceneRenderer::AddFixedColorRenderObj(const RenderObj* renderObj)
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

    void SceneRenderer::AddSplineRenderObj(const RenderObj* renderObj)
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

    void SceneRenderer::AddReflectiveRenderObj(const RenderObj* renderObj, float nearPlane, float reflectiveness)
    {
        RemoveReflectiveRenderObj(renderObj);
        reflectiveRenderObjs_.push_back(renderObj);
        reflectiveMaps_.push_back(ReflectiveMap(256, nearPlane, reflectiveness));
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

    void SceneRenderer::SetDirLight(const DirLight *dirLight)
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
        standardRenderObjs_.clear();
        fixedColorRenderObjs_.clear();
        splineRenderObjs_.clear();
        reflectiveRenderObjs_.clear();
        reflectiveMaps_.clear();
        pointLights_.clear();
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

        const GLShader* gauss5Vert = GLShader::Load("CGFire/gauss5.vert");
        const GLShader* gauss5Frag = GLShader::Load("CGFire/gauss5.frag");
        gauss5Program_ = GLProgram::Load(gauss5Vert, nullptr, gauss5Frag);

        if (postProcess_)
        {
            const GLShader* grainVert = GLShader::Load("CGFire/film_grain.vert");
            const GLShader* grainFrag = GLShader::Load("CGFire/film_grain.frag");
            grainProgram_ = GLProgram::Load(grainVert, nullptr, grainFrag);

            const GLShader* bloomExtractVert = GLShader::Load("CGFire/bloom_extract.vert");
            const GLShader* bloomExtractFrag = GLShader::Load("CGFire/bloom_extract.frag");
            bloomExtractProgram_ = GLProgram::Load(bloomExtractVert, nullptr, bloomExtractFrag);

            const GLShader* bloomCombineVert = GLShader::Load("CGFire/bloom_combine.vert");
            const GLShader* bloomCombineFrag = GLShader::Load("CGFire/bloom_combine.frag");
            bloomCombineProgram_ = GLProgram::Load(bloomCombineVert, nullptr, bloomCombineFrag);
        }

        const GLShader* defDirVert = GLShader::Load("CGFire/deferred_dir.vert");
        const GLShader* defDirFrag = GLShader::Load("CGFire/deferred_dir.frag");
        deferredDirProgram_ = GLProgram::Load(defDirVert, nullptr, defDirFrag);

        const GLShader* defPointVert = GLShader::Load("CGFire/deferred_point.vert");
        const GLShader* defPointFrag = GLShader::Load("CGFire/deferred_point.frag");
        deferredPointProgram_ = GLProgram::Load(defPointVert, nullptr, defPointFrag);
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

    void SceneRenderer::CreateScreenTmpTex()
    {
        glGenTextures(1, &screenTmpTex0_);
        glBindTexture(GL_TEXTURE_2D, screenTmpTex0_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glGenTextures(1, &screenTmpTex1_);
        glBindTexture(GL_TEXTURE_2D, screenTmpTex1_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width_, height_, 0, GL_RGBA, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void SceneRenderer::RenderDirShadow() const
    {
        dirShadowProgram_->Use();
        glm::mat4 lightMat = dirLight_->GetLightMat();
        dirShadowProgram_->SetUniformMat4("light_mat", false, &lightMat[0][0]);

        dirLight_->BindShadowBuffer();
        for (const RenderObj* renderObj : standardRenderObjs_)
            renderObj->RenderPosOnly(dirShadowProgram_);
        for (const RenderObj* renderObj : fixedColorRenderObjs_)
            renderObj->RenderPosOnly(dirShadowProgram_);
        for (const RenderObj* renderObj : reflectiveRenderObjs_)
            renderObj->RenderPosOnly(dirShadowProgram_);

        if (advancedShadow_)
        {
            dirLight_->PrepareGauss5(gauss5Program_);
            glBindVertexArray(fullScreenVao_);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
            glBindVertexArray(0);
        }

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
            for (const RenderObj* renderObj : standardRenderObjs_)
                renderObj->RenderPosOnly(pointShadowProgram_);
            for (const RenderObj* renderObj : fixedColorRenderObjs_)
                renderObj->RenderPosOnly(pointShadowProgram_);
            for (const RenderObj* renderObj : reflectiveRenderObjs_)
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
        for (const RenderObj* renderObj : standardRenderObjs_)
            renderObj->RenderAll(geometryProgram_);

        gBuffer_.Unbind();
    }

    void SceneRenderer::RenderDeferredLighting(glm::vec3 camPos) const
    {
        // Bind screen fbo
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Draw dir
        deferredDirProgram_->Use();
        deferredDirProgram_->SetUniformVec3f("cam_pos", camPos);
        gBuffer_.UseTextures(deferredDirProgram_);

        deferredDirProgram_->SetUniformMat4("light_mat", false, &dirLight_->GetLightMat()[0][0]);
        dirLight_->Use(deferredDirProgram_);
        glActiveTexture(GL_TEXTURE4);
        dirLight_->BindDepthTex();
        deferredDirProgram_->SetUniformI("shadow_tex", 4);
        deferredDirProgram_->SetUniformB("use_esm", advancedShadow_);
        glActiveTexture(GL_TEXTURE5);
        dirLight_->BindEsmTex();
        deferredDirProgram_->SetUniformI("shadow_esm_tex", 5);

        glBindVertexArray(fullScreenVao_);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Draw points
        glDepthFunc(GL_LEQUAL);
        deferredPointProgram_->Use();
        gBuffer_.UseTextures(deferredPointProgram_);
        deferredPointProgram_->SetUniformB("use_esm", advancedShadow_);
        deferredPointProgram_->SetUniformVec3f("cam_pos", camPos);
        for (const PointLight* pointLight : pointLights_)
        {
            // Copy old image
            glCopyTextureSubImage2D(screenTmpTex0_, 0, 0, 0, 0, 0, width_, height_);
            glBindTextureUnit(4, screenTmpTex0_);
            deferredPointProgram_->SetUniformI("old_tex", 4);

            // PointLight specific uniforms
            pointLight->Use(deferredPointProgram_);
            glActiveTexture(GL_TEXTURE5);
            pointLight->BindDepthCubeMap();
            deferredPointProgram_->SetUniformI("shadow_tex", 5);

            // Draw
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        }
        glDepthFunc(GL_LESS);

        // End
        glBindVertexArray(0);
    }

    void SceneRenderer::RenderFixedColor(const float* viewMat, const float* projMat) const
    {
        fixedColorProgram_->Use();
        fixedColorProgram_->SetUniformMat4("view_mat", false, viewMat);
        fixedColorProgram_->SetUniformMat4("proj_mat", false, projMat);
        for (const RenderObj* renderObj : fixedColorRenderObjs_)
            renderObj->RenderDiffuse(fixedColorProgram_);
    }

    void SceneRenderer::RenderSplines(const float *viewMat, const float *projMat) const
    {
        fixedColorProgram_->Use();
        fixedColorProgram_->SetUniformMat4("view_mat", false, viewMat);
        fixedColorProgram_->SetUniformMat4("proj_mat", false, projMat);
        for (const RenderObj* renderObj : splineRenderObjs_)
            renderObj->RenderDiffuse(fixedColorProgram_);
    }

    void SceneRenderer::RenderReflectiveMaps() const
    {
        for (uint32_t i = 0; i < reflectiveMaps_.size(); i++)
        {
            const ReflectiveMap& reflectiveMap = reflectiveMaps_[i];
            const RenderObj* reflectiveRenderObj = reflectiveRenderObjs_[i];

            // Calculate matrices
            uint32_t size = reflectiveMap.GetSize();
            glm::mat4 projMat = reflectiveMap.GetProjMat();
            std::vector<glm::mat4> viewMats = reflectiveMap.GetViewMats(reflectiveRenderObj->GetPos());
            std::vector<glm::mat4> skyboxViewMats = reflectiveMap.GetViewMats(glm::vec3(0.0f));
            float* projMatP = &projMat[0][0];

            // Bind framebuffer
            reflectiveMap.BindFbo();
            glViewport(0, 0, size, size);

            // Draw all 6 faces one by one // TODO: optimize with geometry shader later
            for (uint32_t face = 0; face < 6; face++)
            {
                // Bind and clear buffers
                reflectiveMap.BindCubeMap(); // Cube map need to be bound for every face because RenderSkybox binds skybox cube map
                reflectiveMap.BindCubeMapFace(face);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // Render skybox
                RenderSkybox(&skyboxViewMats[face][0][0], projMatP);

                // Render all reflected objects
                toEnvMapProgram_->Use();
                toEnvMapProgram_->SetUniformMat4("proj_mat", false, projMatP);
                toEnvMapProgram_->SetUniformVec3f("obj_pos", reflectiveRenderObj->GetPos());
                toEnvMapProgram_->SetUniformMat4("view_mat", false, &viewMats[face][0][0]);
                for (const RenderObj* renderObj : standardRenderObjs_)
                    renderObj->RenderDiffuse(toEnvMapProgram_);
                for (const RenderObj* renderObj : fixedColorRenderObjs_)
                    renderObj->RenderDiffuse(toEnvMapProgram_);
                for (const RenderObj* renderObj : reflectiveRenderObjs_)
                {
                    // Dont reflect the reflecting object
                    if (renderObj != reflectiveRenderObj)
                        renderObj->RenderDiffuse(toEnvMapProgram_);
                }
                for (const RenderObj* renderObj : splineRenderObjs_)
                    renderObj->RenderDiffuse(toEnvMapProgram_);
            }

            reflectiveMap.UnbindFbo();
        }
    }

    void SceneRenderer::RenderReflectiveObj(glm::vec3 camPos, const float *viewMat, const float *projMat) const
    {
        for (uint32_t i = 0; i < reflectiveRenderObjs_.size(); i++)
        {
            const RenderObj* reflectiveRenderObj = reflectiveRenderObjs_[i];
            const ReflectiveMap& reflectiveMap = reflectiveMaps_[i];

            reflectiveProgram_->Use();
            reflectiveProgram_->SetUniformMat4("view_mat", false, viewMat);
            reflectiveProgram_->SetUniformMat4("proj_mat", false, projMat);
            reflectiveProgram_->SetUniformVec3f("cam_pos", camPos);
            reflectiveProgram_->SetUniformF("reflectiveness", reflectiveMap.GetReflectiveness());

            glActiveTexture(GL_TEXTURE2);
            reflectiveMap.BindCubeMap();
            reflectiveProgram_->SetUniformI("skybox_tex", 2);
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

    void SceneRenderer::PostProcess(uint32_t width, uint32_t height) const
    {
        // Prepare
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glBindVertexArray(fullScreenVao_);
        glDepthFunc(GL_LEQUAL);

        // Bloom (extract)
        glCopyTextureSubImage2D(screenTmpTex0_, 0, 0, 0, 0, 0, width, height);
        bloomExtractProgram_->Use();
        glBindTextureUnit(0, screenTmpTex0_);
        bloomExtractProgram_->SetUniformI("og_tex", 0);
        bloomExtractProgram_->SetUniformF("min_brightness", 0.71f);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Bloom (blur)
        glCopyTextureSubImage2D(screenTmpTex1_, 0, 0, 0, 0, 0, width, height);
        gauss5Program_->Use();
        glBindTextureUnit(0, screenTmpTex1_);
        gauss5Program_->SetUniformI("og_tex", 0);
        gauss5Program_->SetUniformF("pixel_width", 1.0f / (float) width);
        gauss5Program_->SetUniformF("pixel_height", 1.0f / (float) height);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Bloom (combine)
        glCopyTextureSubImage2D(screenTmpTex1_, 0, 0, 0, 0, 0, width, height);
        bloomCombineProgram_->Use();
        glBindTextureUnit(0, screenTmpTex0_);
        bloomCombineProgram_->SetUniformI("og_tex", 0);
        glBindTextureUnit(1, screenTmpTex1_);
        bloomCombineProgram_->SetUniformI("bloom_tex", 1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Film grain
        glCopyTextureSubImage2D(screenTmpTex0_, 0, 0, 0, 0, 0, width, height);
        grainProgram_->Use();
        glBindTextureUnit(0, screenTmpTex0_);
        grainProgram_->SetUniformI("og_tex", 0);
        grainProgram_->SetUniformF("strength", 0.05f);
        grainProgram_->SetUniformVec2f("rand_in", glm::linearRand(glm::vec2(0.0f), glm::vec2(1.0f)));
        grainProgram_->SetUniformVec2f("scree_size", glm::vec2((float) width, (float) height));
        grainProgram_->SetUniformI("grain_size", 3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Cleanup
        glDepthFunc(GL_LESS);
        glBindVertexArray(0);
    }
}
