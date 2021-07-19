//
// Created by Nika on 31/05/2021.
//based on https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#include "RiggingAndBlending/animation.hpp"
#include "util/assimptoglmmatrix.h"
#include "engine/config.hpp"
#include "engine/Util.hpp"

namespace en{
    Animation::Animation(const std::string& animpath, Model* model){
        std::string realPath = MODEL_ROOT + "/" + animpath;
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(realPath, aiProcess_Triangulate | aiProcess_FlipUVs);
        assert(scene && scene->mRootNode && scene->HasAnimations());
        Duration = scene->mAnimations[0]->mDuration;
        tickspersec = scene->mAnimations[0]->mTicksPerSecond;
        buildhierarchy(rootnode, scene->mRootNode);
        addbones(scene->mAnimations[0], *model);
}
    void Animation::addbones(aiAnimation *anim, Model &model) {
        auto modelbonemap = model.getbonemap();
        for (int i = 0; i<anim->mNumChannels;i++){
            aiNodeAnim* channel = anim->mChannels[i];
            //add code for bones missing from bonemap if necessary
            assert(modelbonemap.find(channel->mNodeName.C_Str())!= modelbonemap.end());
            bones.emplace_back(modelbonemap[channel->mNodeName.C_Str()].boneid, channel, channel->mNodeName.C_Str());
            bonemap = modelbonemap;
        }
    }

    std::map<std::string, boneinfo>& Animation::getbonemap() {
        return bonemap;
    }

    float Animation::gettickspersec() const {
        return tickspersec;
    }
    float Animation::getduration() const {
        return Duration;
    }

    aiNodeStructure& Animation::getrootnode() {
        return rootnode;
    }

    void Animation::buildhierarchy(aiNodeStructure &dest, aiNode *source) {
        dest.name = source->mName.C_Str();
        dest.transformation = util::AssimptoGLM4x4(source->mTransformation);
        dest.numchildren = source->mNumChildren;
        for (int i =0; i< source->mNumChildren; i++){
            aiNodeStructure newnode;
            buildhierarchy(newnode, source->mChildren[i]);
            dest.children.push_back(newnode);
        }
    }
    Bone* Animation::findbone(const std::string& name) {
        for (auto it = std::begin(bones); it != std::end(bones); it++){
            Bone bone = (*it);
            if (bone.getbonename()==name){
                return &(*it);
            }
        }
        return nullptr;
    }
}