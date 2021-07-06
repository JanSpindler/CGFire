//
//Created by vincent on 06.07.2021.
//

#include "engine/Character.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace en{
        Character::Character(const std::string& modelFile, const std::string& characterName)
                : Model(modelFile, true, characterName)
        {
            m_DefaultAnimation = std::make_shared<Animation>(modelFile, this);
            m_Animator = std::make_shared<Animator>(m_DefaultAnimation.get());
            m_Animations["default"] = m_DefaultAnimation;
        }
        void Character::AddAnimation(const std::string& animationFile, const std::string& animationName){
            m_Animations[animationName] = std::make_shared<en::Animation>(animationFile, this);
        };

        void Character::PlayAnimation(const std::string& animationName){
            m_Animator->playAnim(m_Animations[animationName].get());
        }

        void Character::Update(float deltaTime){
            m_Animator->UpdateAnim(deltaTime);
        }
        void Character::Render(const GLProgram* program){
            this->SetBoneUniforms(program);
            Model::Render(program);
        }
        void Character::RenderPosOnly(const GLProgram* program){
            this->SetBoneUniforms(program);
            Model::RenderPosOnly(program);
        }
        void Character::RenderDiffuse(const GLProgram* program){
            this->SetBoneUniforms(program);
            Model::RenderDiffuse(program);
        }
        void Character::RenderAll(const GLProgram* program){
            this->SetBoneUniforms(program);
            Model::RenderAll(program);
        }

        void Character::SetBoneUniforms(const GLProgram* program){
            auto transforms = m_Animator->getfinalbonetransforms();
            for (int i = 0; i < transforms.size(); ++i) {
                program->SetUniformMat4(("finalbones[" + std::to_string(i) + "]").c_str(), false, glm::value_ptr(transforms[i]));
            }
        }
}