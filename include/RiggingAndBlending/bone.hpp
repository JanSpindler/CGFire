//
// Created by Nika on 31/05/2021.
//based on https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#ifndef CGFIRE_BONE_HPP
#define CGFIRE_BONE_HPP
#include <glm/glm.hpp>
#include <vector>
#include <assimp/anim.h>
#include <glm/gtx/quaternion.hpp>

namespace en{
struct TimeQuat {
    float timestamp;
    glm::quat bonequat;
};

struct TimeVec {
    float timestamp;
    glm::vec3 bonevect;
};
class Bone{
    std::vector<TimeQuat> rotation;
    std::vector<TimeVec> scale;
    std::vector<TimeVec> position;
    int numrotation;
    int numscale;
    int numposition;
    std::string bonename;
    glm::mat4 current;
    int boneid;
public:
    Bone(int ID, const aiNodeAnim* channel, const std::string& name);
    void PerFrame(float animationtime);
    const std::string& getbonename();
    const glm::mat4& getcurrent();
private:
    glm::mat4 interppos(float animationtime);
    glm::mat4 interpscale(float animationtime);
    glm::mat4 interprot(float animationtime);
    static float getscale(float animatontime, float current, float next);


};
}

#endif //CGFIRE_BONE_HPP
