
#include "particle/ParticleSystemRenderer.h"
#include <array>
#include <engine/Util.hpp>
#include <util/Random.h>

namespace particle {

    ParticleSystemRenderer::ParticleSystemRenderer(uint32_t particlePoolSize, const en::Camera& cam, bool additiveBlending)
    : m_AdditiveBlending(additiveBlending),
    m_Cam(cam),
    m_MaxQuadCount(particlePoolSize),
    m_MaxIndices(m_MaxQuadCount*6),
    m_MaxVertices(m_MaxQuadCount*4)
    {

        m_QuadData = new Vertex[m_MaxVertices](); //Allocate memory where the vertex data will be written to

        glGenVertexArrays(1, &m_VA);
        glBindVertexArray(m_VA);

        //Vertex Buffer
        glGenBuffers(1, &m_VB);

        glBindBuffer(GL_ARRAY_BUFFER, m_VB);
        glBufferData(GL_ARRAY_BUFFER, m_MaxVertices * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, Position)));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, Color)));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, TexCoord1)));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, TexCoord2)));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, HowMuchOfTexCoord1)));

        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) (offsetof(Vertex, TexID)));

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

        glGenBuffers(1, &m_IB);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_MaxIndices * sizeof(uint32_t), indices, GL_STATIC_DRAW);

        delete[] indices;

        // load and compile shaders and link program
        unsigned int vertexShader = compileShader("particles.vert", GL_VERTEX_SHADER);
        unsigned int fragmentShader = compileShader("particles.frag", GL_FRAGMENT_SHADER);
        m_Shader = linkProgram(vertexShader, fragmentShader);
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);


        // save uniform locations
        m_ShaderViewProj = glGetUniformLocation(m_Shader, "u_ViewProj");

        auto loc = glGetUniformLocation(m_Shader, "u_Textures");

        glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &m_MaxTexturesSlots);

        m_MaxTexturesSlots = std::min(32, m_MaxTexturesSlots);

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

    void ParticleSystemRenderer::InitializeTextures(std::vector<std::shared_ptr<en::GLPictureTex>>& textures){
        //Create the bidirectional mapping from textures to their IDs in the shader
        for (auto& tex : textures){
            m_MapSlotToTexture[m_CurrentMaxTextureSlotIDPlusOne] = tex->getHandle();
            m_MapTextureToSlot[tex->getHandle()] = m_CurrentMaxTextureSlotIDPlusOne;
            m_CurrentMaxTextureSlotIDPlusOne++;
            assert(m_CurrentMaxTextureSlotIDPlusOne <= m_MaxTexturesSlots);

        }
    }
    void ParticleSystemRenderer::BeginBatch(){
        m_QuadIndexCount = 0;
        m_QuadDataPtr = m_QuadData;

        //Make sure the vertex Positions of the quad are set to that the particle normal shows to the camera
        glm::mat4 viewMatrix = m_Cam.GetViewMat();
        auto CamRight = glm::vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
        auto CamUp = glm::vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);
        m_CurrentQuadVertexPositions[0] = glm::vec4(-CamRight - CamUp, 1.f);
        m_CurrentQuadVertexPositions[1] = glm::vec4(CamRight - CamUp, 1.f);
        m_CurrentQuadVertexPositions[2] = glm::vec4(CamRight + CamUp, 1.f);
        m_CurrentQuadVertexPositions[3] = glm::vec4(-CamRight + CamUp, 1.f);
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


        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);

        glBlendFunc(GL_SRC_ALPHA, m_AdditiveBlending ? GL_ONE : GL_ONE_MINUS_SRC_ALPHA);


        //bind textures
        int maxTexSlot = m_CurrentMaxTextureSlotIDPlusOne;
        for (int i = 0; i < maxTexSlot; i++) {
            glActiveTexture(GL_TEXTURE0 + i);

            glBindTexture(GL_TEXTURE_2D, m_MapSlotToTexture[i]);
            //m_MapSlotToTexture[i]->BindTex();
        }

        glUseProgram(m_Shader);

        glm::mat4 viewProjMat = m_Cam.GetViewProjMat();
        glUniformMatrix4fv(m_ShaderViewProj, 1, GL_FALSE, &viewProjMat[0][0]);



        glBindVertexArray(m_VA);
        glDrawElements(GL_TRIANGLES, m_QuadIndexCount, GL_UNSIGNED_INT, nullptr);

        glDisable(GL_BLEND);

        glDepthMask(GL_TRUE);

    }
    void ParticleSystemRenderer::NextBatch(){
        this->Flush();
        this->BeginBatch();
    }

    void ParticleSystemRenderer::DrawParticle(const Particle& particle){

        if (m_QuadIndexCount >= m_MaxIndices) { //Then batch full; next batch
            NextBatch();
        }

        float life;
        if (!particle.TexLooped) //Fire
            life = std::max(particle.LifeRemaining, 0.f) / particle.LifeTime;
        else if (particle.Spline == nullptr) //Water //TODO make water looped
            life = std::max(particle.LifeRemaining - std::floor(particle.LifeRemaining), 0.f) ;
        else //Smoke
            life = 1.f - particle.LifeTime; // in ParticleSystem we saved the parameter t of the spline in LifeTime

        glm::vec4 color = glm::lerp(particle.ColorEnd, particle.ColorBegin, life);
        float size = glm::lerp(particle.SizeEnd, particle.SizeBegin, life);

        //uint8_t textureSlot = 0;//m_MapTextureToSlot[particle.Texture];
        uint8_t textureSlot = m_MapTextureToSlot[particle.Texture->getHandle()];


        //calculate the texture position in the sprite sheet
        auto numSpriteRows = static_cast<uint32_t>(particle.TexCoordAnimFrames.x);
        auto numSpriteColumns = static_cast<uint32_t>(particle.TexCoordAnimFrames.y);
        auto numSpriteFrames = static_cast<uint32_t>(numSpriteRows * numSpriteColumns);

        float howMuchOfTexCoord1;

        uint32_t currentFrame;
        if (!particle.TexLooped) { //water,fire
            float currentFrameFloat = life * numSpriteFrames;
            currentFrame = static_cast<uint32_t>(currentFrameFloat);
            howMuchOfTexCoord1 = currentFrameFloat - currentFrame;
        }
        else{//smoke
            float currentFrameFloat = (0.1f*particle.LifeRemaining - static_cast<uint32_t>(0.1f*particle.LifeRemaining)) * numSpriteFrames;
            currentFrame = static_cast<uint32_t>(currentFrameFloat);
            howMuchOfTexCoord1 = currentFrameFloat - currentFrame;

        }

        uint32_t nextFrame = std::min(currentFrame + 1, numSpriteFrames - 1); //used for smooth blending between frames

        //current frame
        auto currentFrameColumn = currentFrame % numSpriteRows;
        auto currentFrameRow = currentFrame / numSpriteRows;
        glm::vec2 spriteFrameSize = {1.f / numSpriteColumns, 1.f / numSpriteRows};
        glm::vec2 spriteFramePos = {(numSpriteColumns - currentFrameColumn - 1) * spriteFrameSize.x,
                                    currentFrameRow * spriteFrameSize.y};
        glm::vec2 texCoords1[] = {{spriteFramePos.x,                     spriteFramePos.y},
                                  {spriteFramePos.x + spriteFrameSize.x, spriteFramePos.y},
                                  {spriteFramePos.x + spriteFrameSize.x, spriteFramePos.y + spriteFrameSize.y},
                                  {spriteFramePos.x,                     spriteFramePos.y + spriteFrameSize.y}};

        //next frame
        auto nextFrameColumn = nextFrame % numSpriteRows;
        auto nextFrameRow = nextFrame / numSpriteRows;
        spriteFrameSize = {1.f / numSpriteColumns, 1.f / numSpriteRows};
        spriteFramePos = {(numSpriteColumns - nextFrameColumn - 1) * spriteFrameSize.x,
                          nextFrameRow * spriteFrameSize.y};
        glm::vec2 texCoords2[] = {{spriteFramePos.x,                     spriteFramePos.y},
                                  {spriteFramePos.x + spriteFrameSize.x, spriteFramePos.y},
                                  {spriteFramePos.x + spriteFrameSize.x, spriteFramePos.y + spriteFrameSize.y},
                                  {spriteFramePos.x,                     spriteFramePos.y + spriteFrameSize.y}};


        glm::mat4 transform = glm::translate(glm::mat4(1.0f), particle.Position)
                              * glm::scale(glm::mat4(1.0f), { size, size, size });

        for (size_t i = 0; i < 4; ++i)
        {
            m_QuadDataPtr->Position = transform * m_CurrentQuadVertexPositions[i];
            m_QuadDataPtr->Color = color;
            m_QuadDataPtr->TexCoord1 = texCoords1[i];
            m_QuadDataPtr->TexCoord2 = texCoords2[i];
            m_QuadDataPtr->HowMuchOfTexCoord1 = howMuchOfTexCoord1;
            m_QuadDataPtr->TexID = textureSlot;
            m_QuadDataPtr++;
        }

        m_QuadIndexCount += 6; //jump 6 addresses for the 6 indices
    }
}