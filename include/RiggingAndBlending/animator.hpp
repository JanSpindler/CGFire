//
// Created by Nika on 03/06/2021.
//based on https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#ifndef CGFIRE_ANIMATOR_HPP
#define CGFIRE_ANIMATOR_HPP
#define MAXBONENUM 100
#include "animation.hpp"

namespace en{
    class Animator{
        float currenttime;
        float deltatime;
        Animation* current;
        std::vector<glm::mat4> finalbonetransforms;



        void BoneTransform(aiNodeStructure& node, const glm::mat4& parenttransform);

    public:
        explicit Animator(Animation* currentanim);
        void UpdateAnim(float dt);
        const std::vector<glm::mat4>& getfinalbonetransforms();

        void playAnim(Animation* anim);
    };
}

#endif //CGFIRE_ANIMATOR_HPP
