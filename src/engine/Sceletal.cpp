//
//Created by vincent on 06.07.2021.
//

#include "engine/Sceletal.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace en{
        Sceletal::Sceletal(const std::string& modelFile, const std::string& characterName)
                : Model(modelFile, true, characterName)
        {
            m_DefaultAnimation = std::make_shared<Animation>(modelFile, this);
            m_Animator = std::make_shared<Animator>(m_DefaultAnimation.get());
            m_Animations["default"] = m_DefaultAnimation;
        }
        void Sceletal::AddAnimation(const std::string& animationFile, const std::string& animationName){
            m_Animations[animationName] = std::make_shared<en::Animation>(animationFile, this);
        };

        void Sceletal::PlayAnimation(const std::string& animationName){
            m_Animator->playAnim(m_Animations[animationName].get());
        }

        void Sceletal::Update(float deltaTime){
            m_Animator->UpdateAnim(deltaTime);
        }
        void Sceletal::Render(const GLProgram* program){
            this->SetBoneUniforms(program);
            Model::Render(program);
        }
        void Sceletal::RenderPosOnly(const GLProgram* program){
            this->SetBoneUniforms(program);
            Model::RenderPosOnly(program);
        }
        void Sceletal::RenderDiffuse(const GLProgram* program){
            this->SetBoneUniforms(program);
            Model::RenderDiffuse(program);
        }
        void Sceletal::RenderAll(const GLProgram* program){
            this->SetBoneUniforms(program);
            Model::RenderAll(program);
        }

        void Sceletal::SetBoneUniforms(const GLProgram* program){
            auto transforms = m_Animator->getfinalbonetransforms();
            for (int i = 0; i < transforms.size(); ++i) {
                program->SetUniformMat4(("finalbones[" + std::to_string(i) + "]").c_str(), false, glm::value_ptr(transforms[i]));
            }
        }
}