//
// Created by Nika on 31/05/2021.
//based on https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#include "RiggingAndBlending/bone.hpp"
#include "util/assimptoglmmatrix.h"
#include <glm/gtx/quaternion.hpp>
#include "engine/Util.hpp"

namespace en{
    Bone::Bone(int ID, const aiNodeAnim *channel, std::string name) {
        boneid = ID;
        bonename = name;
        current = glm::mat4(1.0f);
        numrotation = channel->mNumRotationKeys;
        numposition = channel->mNumPositionKeys;
        numscale = channel->mNumScalingKeys;
        Log::Info("Bone"+ name +"has scale size"+ std::to_string(numscale));
        for(int i = 0; i<numposition;i++ ){
            TimeVec newvec{};
            newvec.timestamp = channel->mPositionKeys[i].mTime;
            newvec.bonevect = util::AssimptoGLMvec3(channel->mPositionKeys[i].mValue);
            position.push_back(newvec);
        }
        for (int k = 0; k<numscale;k++){
            TimeVec newvec{};
            newvec.timestamp = channel->mScalingKeys[k].mTime;
            newvec.bonevect = util::AssimptoGLMvec3(channel->mScalingKeys[k].mValue);
            scale.push_back(newvec);
        }
        for (int l = 0; l<numrotation; l++){
            TimeQuat newquat{};
            newquat.timestamp = channel->mRotationKeys[l].mTime;
            newquat.bonequat = util::AssimptoGLmquat(channel->mRotationKeys[l].mValue);
            rotation.push_back(newquat);
        }
    }
    glm::mat4 Bone::getcurrent() {
        return current;
    }
    void Bone::PerFrame(float animationtime) {
        current = interppos(animationtime)* interprot(animationtime)* interpscale(animationtime);
    }
     float Bone::getscale(float animationtime, float current, float next) {
        return (animationtime-current)/(next-current);
    }
    glm::mat4 Bone::interppos(float animationtime) {
        if (numposition == 1){return glm::translate(glm::mat4(1.0f), position[0].bonevect);}
        int i = -1;
        for(int k = 0; k<numposition-1; k++){
            if(position[k+1].timestamp > animationtime){
                i = k;
                break;
            }
        }
        assert(i>=0);
        return glm::translate(glm::mat4(1.0f), glm::mix(position[i].bonevect, position[i+1].bonevect, getscale(animationtime, position[i].timestamp, position[i+1].timestamp)));
    }
    glm::mat4 Bone::interprot(float animationtime) {
        Log::Info("looking through rotation size" + std::to_string(numrotation));
        if (numrotation == 1) { return glm::toMat4(glm::normalize(rotation[0].bonequat)); }
        int i = -1;
        for(int k = 0; k<numrotation-1; k++){
            if(rotation[k+1].timestamp > animationtime){
                i = k;
                break;
            }
        }
        assert(i>=0);
        return glm::toMat4(glm::normalize(glm::slerp(rotation[i].bonequat, rotation[i+1].bonequat, getscale(animationtime, rotation[i].timestamp, rotation[i+1].timestamp))));
    }
    glm::mat4 Bone::interpscale(float animationtime) {
        Log::Info("looking through scale size" + std::to_string(numscale));
        if (numscale == 1){return glm::scale(glm::mat4(1.0f),scale[0].bonevect);}
        int i = -1;
        for(int k = 0; k<numscale-1; k++){
            if(scale[k+1].timestamp > animationtime){
                i = k;
                break;
            }
        }
        Log::Info("int is"+ std::to_string(i));
        assert(i>=0);
        return glm::scale(glm::mat4(1.0f), glm::mix(scale[i].bonevect, scale[i+1].bonevect, getscale(animationtime, scale[i].timestamp, scale[i+1].timestamp)));
    }
    std::string Bone::getbonename() {
        return bonename;
    }
}

