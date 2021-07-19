//
// Created by Nika on 31/05/2021.
//based on https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#ifndef CGFIRE_ANIMATION_HPP
#define CGFIRE_ANIMATION_HPP

#include <glm/glm.hpp>
#include <vector>
#include <assimp/anim.h>
#include <glm/gtx/quaternion.hpp>
#include "bone.hpp"
#include "engine/model/Model.hpp"
#include <map>

namespace en{
    struct aiNodeStructure{
        std::vector<aiNodeStructure> children;
        int numchildren;
        glm::mat4 transformation;
        std::string name;
    };
    class Animation{
        std::vector<Bone> bones;
        aiNodeStructure rootnode;
        float Duration;
        float tickspersec;
        std::map<std::string, boneinfo> bonemap;
    public:
        Animation(const std::string& animpath, Model *model);
        Bone* findbone(const std::string& name);
        std::map<std::string, boneinfo>& getbonemap();
        float gettickspersec() const;
        float getduration() const;
        aiNodeStructure& getrootnode();
    private:
        void buildhierarchy(aiNodeStructure& dest, aiNode* source);
        void addbones(aiAnimation* anim, Model& model);
    };

}

#endif //CGFIRE_ANIMATION_HPP
