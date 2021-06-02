//
//Created by vincent on 29.05.2021.
//

#pragma once


namespace particle{
    struct ParticleProps {
        glm::vec3 Position;
        glm::vec3 PositionVariation = {0.f, 0.f, 0.f};
        glm::vec3 Velocity;
        //for random variable r between -0.5 and 0.5, the velocity will be varied by r * VelocityVariation
        glm::vec3 VelocityVariation;
        glm::vec4 ColorBegin, ColorEnd;
        float SizeBegin, SizeEnd;
        //for random variable r between -0.5 and 0.5, the size will be varied by r * SizeVariation
        float SizeVariation;
        float LifeTime = 1.0f; // in Seconds
        float LifeTimeVariation = 0.f;
        en::GLPictureTex* Texture = nullptr;

        glm::vec2 TexCoord = {0.f, 0.f}; //If you want to use animation you can change the TexCoord for each frame
        glm::vec2 TexCoordAnimFrames = {1.f, 1.f}; // rows, columns of the sprite-sheet

        int ParticlesPerEmit = 10; //int because imgui does not support unsigned int
    };

    struct Particle {
        glm::vec3 Position;
        glm::vec3 Velocity;
        glm::vec4 ColorBegin, ColorEnd;
        float Rotation = 0.0f;
        float SizeBegin, SizeEnd;

        float LifeTime = 1.0f;
        float LifeRemaining = 1.0f;

        en::GLPictureTex* Texture = nullptr;

        glm::vec2 TexCoord = {0.f, 0.f};
        glm::vec2 TexCoordAnimFrames = {1.f, 1.f}; // rows, columns of the sprite-sheet


        bool Active = false;

        float CameraDistance = -1.0f; // *Squared* distance to the camera. if dead : -1.0f
        bool operator<(const Particle& that) const {
            // Sort in reverse order : far particles drawn first.
            return this->CameraDistance > that.CameraDistance;
        }
    };
}