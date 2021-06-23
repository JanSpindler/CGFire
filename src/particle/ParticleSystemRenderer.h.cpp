//
// Created by JS on 03/06/2021.
//

#include "particle/ParticleSystemRenderer.h"
#include <array>
#include <engine/Util.hpp>
#include <util/OpenGLDebug.h>

namespace particle {

    ParticleSystemRenderer::ParticleSystemRenderer(uint32_t particlePoolSize, const en::Camera& cam)
            : m_Cam(cam),
              m_MaxQuadCount(particlePoolSize),
              m_MaxIndices(m_MaxQuadCount*6),
              m_MaxVertices(m_MaxQuadCount*4)
    {
        m_QuadData = new Vertex[m_MaxVertices](); //Allocate memory where the vertex data will be written to

        glCreateVertexArrays(1, &m_VA);
        glBindVertexArray(m_VA);

        //Vertex Buffer
        glCreateBuffers(1, &m_VB);

        glBindBuffer(GL_ARRAY_BUFFER, m_VB);
        glBufferData(GL_ARRAY_BUFFER, m_MaxVertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexArrayAttrib(m_VA, 0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, Position)));

        glEnableVertexArrayAttrib(m_VA, 1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, Color)));

        glEnableVertexArrayAttrib(m_VA, 2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, TexCoord)));

        glEnableVertexArrayAttrib(m_VA, 3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, TexID)));

        //Set up index buffer which we will never change later
        uint32_t* indices = new uint32_t[m_MaxIndices];

        uint32_t offset = 0;
        for (size_t i = 0; i < m_MaxIndices; i += 6){
            indices[i + 0] = 0 + offset;
            indices[i + 1] = 1 + offset;
            indices[i + 2] = 2 + offset;

            indices[i + 3] = 2 + offset;
            indices[i + 4] = 3 + offset;
            indices[i + 5] = 0 + offset;

            offset += 4;
        }

        glCreateBuffers(1, &m_IB);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_MaxIndices * sizeof(uint32_t), indices, GL_STATIC_DRAW);

        delete[] indices;

        // load and compile shaders and link program
        unsigned int vertexShader = compileShader("CGFire/particles.vert", GL_VERTEX_SHADER);
        unsigned int fragmentShader = compileShader("CGFire/particles.frag", GL_FRAGMENT_SHADER);
        m_Shader = linkProgram(vertexShader, fragmentShader);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);

        // save uniform locations
        m_ShaderViewProj = glGetUniformLocation(m_Shader, "u_ViewProj");
        m_ShaderCamUp = glGetUniformLocation(m_Shader, "u_CameraUp");
        m_ShaderCamUp = glGetUniformLocation(m_Shader, "u_CameraRight");

        auto loc = glGetUniformLocation(m_Shader, "u_Textures");

        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_MaxTexturesSlots);

        for (int i = 0; i < m_MaxTexturesSlots; ++i)
            m_ShaderSamplers[i] = i;

        glUseProgram(m_Shader);
        glUniform1iv(loc, 32, m_ShaderSamplers);

    }
    ParticleSystemRenderer::~ParticleSystemRenderer() {
        glDeleteBuffers(1, &m_VA);
        glDeleteBuffers(1, &m_VB);
        glDeleteBuffers(1, &m_IB);
        delete[] m_QuadData;
    }

    void ParticleSystemRenderer::initializeTextures(std::vector<std::shared_ptr<en::GLPictureTex>>& textures){
        //Create the bidirectional mapping from textures to their IDs in the shader
        for (auto& tex : textures){
            m_MapSlotToTexture[m_CurrentMaxTextureSlotIDPlusOne] = tex.get();
            m_MapTextureToSlot[tex.get()] = m_CurrentMaxTextureSlotIDPlusOne;
            m_CurrentMaxTextureSlotIDPlusOne++;
            assert(m_CurrentMaxTextureSlotIDPlusOne <= m_MaxTexturesSlots);

        }
    }
    void ParticleSystemRenderer::BeginBatch(){
        m_QuadIndexCount = 0;
        m_QuadDataPtr = m_QuadData;
    }

    void ParticleSystemRenderer::EndBatch(){
        Flush();
    }

    void ParticleSystemRenderer::Flush(){
        if (m_QuadIndexCount == 0) // if nothing to draw, return
            return;

        auto dataSize = static_cast<uint32_t>((m_QuadDataPtr - m_QuadData) * sizeof(Vertex));


        //update vertex buffer data
        glBindBuffer(GL_ARRAY_BUFFER, m_VB);
        glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, m_QuadData);


        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);


        //bind textures
        for (int i = 0; i < m_CurrentMaxTextureSlotIDPlusOne; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            m_MapSlotToTexture[i]->BindTex();
        }

        glUseProgram(m_Shader);

        glm::mat4 viewMat = m_Cam.GetViewMat();
        glm::mat4 projMat = m_Cam.GetProjMat();
        glm::mat4 viewProjMat = projMat * viewMat;
        glUniformMatrix4fv(m_ShaderViewProj, 1, GL_FALSE, &viewProjMat[0][0]);



        glBindVertexArray(m_VA);
        glDrawElements(GL_TRIANGLES, m_QuadIndexCount, GL_UNSIGNED_INT, nullptr);

        glDisable(GL_BLEND);

    }
    void ParticleSystemRenderer::NextBatch(){
        this->Flush();
        this->BeginBatch();
    }

    void ParticleSystemRenderer::DrawParticle(const Particle& particle){

        if (m_QuadIndexCount >= m_MaxIndices) { //Then batch full; next batch
            NextBatch();
        }

        float life = particle.LifeRemaining / particle.LifeTime;
        glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);
        float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);

        uint8_t textureSlot = m_MapTextureToSlot[particle.Texture];


        //calculate the texture position in the sprite sheet
        auto numSpriteRows = static_cast<uint32_t>(particle.TexCoordAnimFrames.x);
        auto numSpriteColumns = static_cast<uint32_t>(particle.TexCoordAnimFrames.y);
        auto numSpriteFrames = static_cast<uint32_t>(numSpriteRows * numSpriteColumns);
        auto currentFrame = static_cast<uint32_t>(life * numSpriteFrames);
        auto currentFrameColumn = currentFrame % numSpriteRows;
        auto currentFrameRow = currentFrame / numSpriteRows;
        glm::vec2 spriteFrameSize = {1.f / numSpriteColumns, 1.f / numSpriteRows};
        glm::vec2 spriteFramePos = {currentFrameColumn * spriteFrameSize.x, currentFrameRow * spriteFrameSize.y};
        glm::vec2 texCoords[] = {     { spriteFramePos.x, spriteFramePos.y },
                                      { spriteFramePos.x + spriteFrameSize.x, spriteFramePos.y },
                                      { spriteFramePos.x + spriteFrameSize.x, spriteFramePos.y + spriteFrameSize.y },
                                      { spriteFramePos.x, spriteFramePos.y + spriteFrameSize.y } };



        glm::mat4 viewMatrix = m_Cam.GetViewMat();
        auto CamRight = glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
        auto CamUp = glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

        glm::vec4 quadVertexPositions[] = {
                {  -0.5f, -0.5f, 0.0f, 1.0f },
                {  0.5f, -0.5f, 0.0f, 1.0f },
                {  0.5f,  0.5f, 0.0f, 1.0f },
                { -0.5f,  0.5f, 0.0f, 1.0f } };



        glm::mat4 transform = glm::translate(glm::mat4(1.0f), particle.Position)
                              * glm::scale(glm::mat4(1.0f), { size, size, 1.0f });

        for (size_t i = 0; i < 4; ++i)
        {
            m_QuadDataPtr->Position = transform * quadVertexPositions[i];
            m_QuadDataPtr->Color = color;
            m_QuadDataPtr->TexCoord = texCoords[i];
            m_QuadDataPtr->TexID = textureSlot;
            m_QuadDataPtr++;
        }

        m_QuadIndexCount += 6; //jump 6 addresses for the 6 indices
    }
}