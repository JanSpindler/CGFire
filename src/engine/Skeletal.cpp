//
//Created by vincent on 06.07.2021.
//

#include "engine/Skeletal.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace en{
        Skeletal::Skeletal(const std::string& modelFile)
                : Model(modelFile, true)
        {
            m_DefaultAnimation = std::make_shared<Animation>(modelFile, this);
            m_Animator = std::make_shared<Animator>(m_DefaultAnimation.get());
            m_Animations["default"] = m_DefaultAnimation;
        }
        void Skeletal::AddAnimation(const std::string& animationFile, const std::string& animationName){
            if (!m_Animations.count(animationName)) {
                m_Animations[animationName] = std::make_shared<en::Animation>(animationFile, this);
                m_AnimationsNamesAndFiles.emplace_back(std::make_pair(animationName, animationFile));
            }
        };
        void Skeletal::DeleteAnimation(const std::string& animationName){
            for (auto it = m_AnimationsNamesAndFiles.begin(); it != m_AnimationsNamesAndFiles.end(); ++it){
                if (it->first == animationName){
                    m_AnimationsNamesAndFiles.erase(it);
                    break;
                }
            }
        }
        void Skeletal::PlayAnimation(const std::string& animationName){
            if (m_Animations.count(animationName))
                m_Animator->playAnim(m_Animations[animationName].get());
        }

        void Skeletal::Update(float deltaTime){
            m_Animator->UpdateAnim(deltaTime);
        }
        void Skeletal::Render(const GLProgram* program){
            this->SetBoneUniforms(program);
            Model::Render(program);
        }
        void Skeletal::RenderPosOnly(const GLProgram* program){
            this->SetBoneUniforms(program);
            Model::RenderPosOnly(program);
        }
        void Skeletal::RenderDiffuse(const GLProgram* program){
            this->SetBoneUniforms(program);
            Model::RenderDiffuse(program);
        }
        void Skeletal::RenderAll(const GLProgram* program){
            this->SetBoneUniforms(program);
            Model::RenderAll(program);
        }

        void Skeletal::SetBoneUniforms(const GLProgram* program){
            auto transforms = m_Animator->getfinalbonetransforms();
            for (int i = 0; i < transforms.size(); ++i) {
                program->SetUniformMat4(("finalbones[" + std::to_string(i) + "]").c_str(), false, glm::value_ptr(transforms[i]));
            }
        }
        bool Skeletal::IsRenderObjTypePossible(en::RenderObjType type) const{
            switch(type){
                case RenderObjType::Skeletal:
                case RenderObjType::FixedColor:
                case RenderObjType::Reflective:
                    return true;
                default:
                    return false;
            }
        }
}