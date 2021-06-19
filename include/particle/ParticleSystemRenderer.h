//
//Created by vincent on 26.05.2021.
//

#pragma once

#include <glad/glad.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>
#include "util/TextureLoading ToBeDeleted.h"

#include "framework/shader.hpp"
#include "engine/GLTexture.hpp"
#include "particle/Particle.h"

#include "engine/Camera.hpp"

#include <unordered_map>


namespace particle {
    struct Vertex {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        float TexID;
    };

    /**A batch renderer which makes one draw-call for all the particles of the Pool*/
    class ParticleSystemRenderer {
    public:
        explicit ParticleSystemRenderer(uint32_t particlePoolSize, const en::Camera& cam);
        ~ParticleSystemRenderer();

        //Needs to be called with all the textures that will be used
        void InitializeTextures(std::vector<std::shared_ptr<en::GLPictureTex>>& textures);

        //Needs to be called at the start of a new rendering
        void BeginBatch();

        //in between BeginBatch() and EndBatch() call this for each particle to be drawn
        void DrawParticle(const Particle& particle);

        //draws all the particles of the current batch and clears the buffer
        void EndBatch();
    private:
        //the actual draw and clear method. It is used in EndBatch() and in NextBatch()
        void Flush();

        //in DrawParticle(), once the current batch is full, this is called. It will Flush() and BeginBatch()
        void NextBatch();


    private:
        GLint m_MaxTexturesSlots;
        const en::Camera& m_Cam;

        GLuint m_MaxQuadCount, m_MaxIndices, m_MaxVertices;
        Vertex* m_QuadData; // the data that will be passed to the vertex buffer
        GLuint m_QuadIndexCount; // Current Quad index
        Vertex* m_QuadDataPtr; // Pointer to current quad

        std::unordered_map<en::GLPictureTex*, uint8_t> m_MapTextureToSlot;
        std::unordered_map<uint8_t, en::GLPictureTex*> m_MapSlotToTexture;
        uint8_t m_CurrentMaxTextureSlotIDPlusOne = 0;

        /***The Quad rotated to fit the current camera setup.
         * It will be set once every BeginBatch()*/
        glm::vec4 m_CurrentQuadVertexPositions[4];

        GLuint m_VA;
        GLuint m_VB;
        GLuint m_IB;

        GLuint m_Shader;
        GLint m_ShaderViewProj;
        GLint m_ShaderCamUp;
        GLint m_ShaderCamRight;
        GLint m_ShaderSamplers[32];

    };
}
