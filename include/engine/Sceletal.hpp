//
//Created by vincent on 06.07.2021.
//

#include <RiggingAndBlending/animator.hpp>
#include <memory>

#pragma once
namespace en {
    // Each Object in the scene that has animations will be of this type.
    class Sceletal : public en::Model{
        friend class CharacterAnimationManager;
    public:
        /**The modelFile must at the same time include the default animation*/
        explicit Sceletal(const std::string& modelFile);
        void AddAnimation(const std::string& animationFile, const std::string& animationName);
        void PlayAnimation(const std::string& animationName);

        void Update(float deltaTime);
        void Render(const GLProgram* program) override;
        void RenderPosOnly(const GLProgram* program) override;
        void RenderDiffuse(const GLProgram* program) override;
        void RenderAll(const GLProgram* program) override;

    private:
        std::shared_ptr<en::Animator> m_Animator;
        std::shared_ptr<Animation> m_DefaultAnimation;
        std::unordered_map<std::string, std::shared_ptr<Animation>> m_Animations;

        void SetBoneUniforms(const GLProgram* program);
    };
}
