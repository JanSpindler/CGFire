//
// Created by Nika on 03/06/2021.
//based on https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#include "RiggingAndBlending/animator.hpp"
#include <math.h>
#include "engine/Util.hpp"
#include <glm/gtx/string_cast.hpp>


namespace en{
    Animator::Animator(Animation* currentanim) {
        current = currentanim;
        currenttime = 0.0;
        deltatime = 0.0;
        finalbonetransforms.reserve(MAXBONENUM);
        for (int i = 0; i<MAXBONENUM; i++){
            finalbonetransforms.emplace_back(1.0f);
        }
    }
    void Animator::playAnim(Animation *anim) {
        current = anim;
        currenttime = 0.0f;
    }

    void Animator::BetterBoneTransform() {
        //glm::mat4 parenttransform = glm::mat4(1.0f);
        for (int i=0;i<current->hierarchy.size();i++){
            betterNode node = current->hierarchy[i];
            Bone* bone = node.bone;
            glm::mat4 nodetransform = node.transformation;
            if (bone){
                //Log::Info("Found bone" + bone->getbonename());
                //printf("not: %s\n", bone->getbonename().c_str());
                bone->PerFrame(currenttime);
                nodetransform = bone->getcurrent();
            }
            //Log::Info("gotcurrent");
            glm::mat4 global;
            glm::mat4 parenttransform;
            if(node.parentid>=0) {
                global = current->hierarchy[node.parentid].currenttransform * nodetransform;
                parenttransform = current->hierarchy[node.parentid].currenttransform;
            }
            else{
                //printf("root");
                global = nodetransform;
                parenttransform = glm::mat4(1.0f);
            }
            auto boneinfomap = current->getbonemap();
            //Log::Info("got bonemap");
            if (boneinfomap.find(node.name)!= boneinfomap.end()) {
                int index = boneinfomap[node.name].boneid;
                //Log::Info("found index"+std::to_string(index));
                assert(boneinfomap[node.name].boneid<MAXBONENUM);
                finalbonetransforms[boneinfomap[node.name].boneid] = global * boneinfomap[node.name].offsetmat;
                //Log::Info("setfinaltransform");
                //printf("%s", glm::to_string(finalbonetransforms[boneinfomap[node.name].boneid]).c_str());
            }
            current->hierarchy[i].currenttransform = global;
            //printf("not: %s\n", glm::to_string(parenttransform).c_str());
        }
    }
    void Animator::BoneTransform(aiNodeStructure node, glm::mat4 parenttransform) {
        //Log::Info("Transforming Node" + node.name);
        Bone* bone = current->findbone(node.name);
        glm::mat4 nodetransform = node.transformation;
        //in case model is sideways GlobalInverseTransform = rootnode.transformation may need to be inverted?
        //if (node.name == "RootNode"){
        //    nodetransform = inverse(nodetransform);
        //    Log::Info("inverted root");
        //}
        if (bone){
            printf("working : %s\n", bone->getbonename().c_str());
            //Log::Info("Found bone" + bone->getbonename());
            bone->PerFrame(currenttime);
            nodetransform = bone->getcurrent();
        }
        //Log::Info("gotcurrent");
        glm::mat4 global = parenttransform * nodetransform;
        printf("working: %s\n", glm::to_string(parenttransform).c_str());
        auto boneinfomap = current->getbonemap();
        //Log::Info("got bonemap");
        if (boneinfomap.find(node.name)!= boneinfomap.end()) {
            int index = boneinfomap[node.name].boneid;
            //Log::Info("found index"+std::to_string(index));
            assert(boneinfomap[node.name].boneid<MAXBONENUM);
            finalbonetransforms[boneinfomap[node.name].boneid] = global * boneinfomap[node.name].offsetmat;
            //Log::Info("setfinaltransform");
        }
        for (int i=0; i<node.numchildren; i++){
            //Log::Info("calling children");
            BoneTransform(node.children[i], global);
        }
    }
    void Animator::UpdateAnim(float dt) {
        deltatime = dt;
        if(current){
            currenttime = fmod(currenttime + current->gettickspersec()*deltatime, current->getduration());
            //BoneTransform(current->getrootnode(), glm::mat4(1.0f));
            BetterBoneTransform();
        }
    }
    std::vector<glm::mat4> Animator::getfinalbonetransforms() {
        return finalbonetransforms;
    }
}

