
#include "particle/ParticleSystemRenderer.h"
#include <array>
#include <engine/Util.hpp>

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

        glEnableVertexArrayAttrib(m_VB, 0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, Position)));

        glEnableVertexArrayAttrib(m_VB, 1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, Color)));

        glEnableVertexArrayAttrib(m_VB, 2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, TexCoord)));

        glEnableVertexArrayAttrib(m_VB, 3);
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
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

        delete[] indices;

        // load and compile shaders and link program
        unsigned int vertexShader = compileShader("particles.vert", GL_VERTEX_SHADER);
        unsigned int fragmentShader = compileShader("particles.frag", GL_FRAGMENT_SHADER);
        m_Shader = linkProgram(vertexShader, fragmentShader);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);

        glUseProgram(m_Shader);
        // save uniform locations
        m_ShaderViewProj = glGetUniformLocation(m_Shader, "u_ViewProj");

        auto loc = glGetUniformLocation(m_Shader, "u_Textures");
        for (int i = 0; i < 32; ++i)
            m_ShaderSamplers[i] = i;
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
            m_MapSlotToTexture[m_MaxTextureSlotIDPlusOne] = tex.get();
            m_MapTextureToSlot[tex.get()] = m_MaxTextureSlotIDPlusOne;
            m_MaxTextureSlotIDPlusOne++;

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
        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);


        //bind textures
        for (int i = 0; i < m_MaxTextureSlotIDPlusOne; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            m_MapSlotToTexture[i]->Bind();
        }

        glUseProgram(m_Shader);
        glm::mat4 viewProjMat = m_Cam.GetViewProjMat();
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

        //float life = particle.LifeRemaining / particle.LifeTime;
       // glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);
        //float size = glm::lerp(particle.SizeBegin, particle.SizeEnd, life);
        uint8_t textureSlot = m_MapTextureToSlot[particle.Texture];

        float size = 1;
        glm::vec4 color = particle.ColorBegin;

        constexpr glm::vec4 quadVertexPositions[] = {{ -0.5f, -0.5f, 0.0f, 1.0f },
                                                    {  0.5f, -0.5f, 0.0f, 1.0f },
                                                    {  0.5f,  0.5f, 0.0f, 1.0f },
                                                    { -0.5f,  0.5f, 0.0f, 1.0f } };
        constexpr glm::vec2 texCoords[] = {     { 0.0f, 0.0f },
                                                { 1.0f, 0.0f },
                                                { 1.0f, 1.0f },
                                                { 0.0f, 1.0f } };

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