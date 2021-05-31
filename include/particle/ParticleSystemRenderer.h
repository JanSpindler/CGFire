//
//Created by vincent on 26.05.2021.
//

#pragma once

#include <glad/glad.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>
#include "util/TextureLoading.h"

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
        void initializeTextures(std::vector<std::shared_ptr<en::GLPictureTex>>& textures);

        void BeginBatch();
        void DrawParticle(const Particle& particle);
        void EndBatch();
    private:
        void Flush();
        void NextBatch();


    private:
        const en::Camera& m_Cam;

        GLuint m_MaxQuadCount, m_MaxIndices, m_MaxVertices;
        Vertex* m_QuadData; // the data that will be passed to the vertex buffer
        GLuint m_QuadIndexCount; // Current Quad index
        Vertex* m_QuadDataPtr; // Pointer to current quad

        std::unordered_map<en::GLPictureTex*, uint8_t> m_MapTextureToSlot;
        std::unordered_map<uint8_t, en::GLPictureTex*> m_MapSlotToTexture;
        uint8_t m_MaxTextureSlotIDPlusOne = 0;

        GLuint m_VA;
        GLuint m_VB;
        GLuint m_IB;

        GLuint m_Shader;
        GLint m_ShaderViewProj;
        GLint m_ShaderSamplers[32];

    };
}
