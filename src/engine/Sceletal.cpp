//
//Created by vincent on 06.07.2021.
//

#include "engine/Sceletal.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "engine/Util.hpp"
#include <glm/gtx/string_cast.hpp>


namespace en{
        Sceletal::Sceletal(const std::string& modelFile, const std::string& characterName)
                : Model(modelFile, true, false, characterName)
        {
            m_DefaultAnimation = std::make_shared<Animation>(modelFile, this);
            m_Animator = std::make_shared<Animator>(m_DefaultAnimation.get());
            m_Animations["default"] = m_DefaultAnimation;
            m_Animator->UpdateAnim(0);
            auto current = m_Animator->getfinalbonetransforms();
            for (int i = 0; i<current.size(); i++){
                prevbonetransforms.emplace_back(1.0f);
                prevbonetransforms[i] = current[i];
            }
        }
        void Sceletal::AddAnimation(const std::string& animationFile, const std::string& animationName){
            m_Animations[animationName] = std::make_shared<en::Animation>(animationFile, this);
        };

        void Sceletal::PlayAnimation(const std::string& animationName){
            m_Animator->playAnim(m_Animations[animationName].get());
            m_Animator->UpdateAnim(0);
            auto current = m_Animator->getfinalbonetransforms();
            for (int i = 0; i<current.size(); i++){
                prevbonetransforms[i] = current[i];
            }
        }

        void Sceletal::Update(float deltaTime){
            auto current = m_Animator->getfinalbonetransforms();
            for (int i = 0; i<current.size(); i++){
                prevbonetransforms[i] = current[i];
            }
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
            //printf("1: %s\n2: %s\n",glm::to_string(transforms[0]).c_str(),glm::to_string(prevbonetransforms[0]).c_str());
            //printf("2: %s", glm::to_string(prevbonetransforms[0]).c_str());
            for (int i = 0; i < transforms.size(); ++i) {
                assert(transforms.size()==prevbonetransforms.size());
                program->SetUniformMat4(("finalbones[" + std::to_string(i) + "]").c_str(), false, glm::value_ptr(transforms[i]));
                program->SetUniformMat4(("prevfinalbones[" + std::to_string(i) + "]").c_str(), false, glm::value_ptr(prevbonetransforms[i]));
                //prevbonetransforms[i] = transforms[i];
            }
        }
}