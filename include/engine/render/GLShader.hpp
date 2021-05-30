//
// Created by JS on 23/05/2021.
//

#ifndef CGFIRE_SHADER_H
#define CGFIRE_SHADER_H

#include <glm/glm.hpp>
#include <string>

namespace en
{
    class GLShader
    {
    public:
        enum class Type
        {
            VERTEX = 0,
            FRAGMENT = 1
        };

        GLShader(const std::string& filename, Type type);
        ~GLShader();

        void AttachTo(unsigned int program) const;
        void Delete() const;

    private:
        unsigned int handle_;
    };

    class GLProgram
    {
    public:
        GLProgram(const GLShader& vertShader, const GLShader& fragShader);
        ~GLProgram();

        void Use() const;
        void Delete() const;

        void SetUniformF(const std::string& name, float val) const;
        void SetUniformI(const std::string& name, int val) const;
        void SetUniformUI(const std::string& name, unsigned int val) const;
        void SetUniformB(const std::string& name, bool val) const;

        void SetUniformVec2f(const std::string& name, const glm::vec2& val) const;
        void SetUniformVec3f(const std::string& name, const glm::vec3& val) const;
        void SetUniformVec4f(const std::string& name, const glm::vec4& val) const;

        void SetUniformMat2(const std::string& name, bool transpose, const float* data) const;
        void SetUniformMat3(const std::string& name, bool transpose, const float* data) const;
        void SetUniformMat4(const std::string& name, bool transpose, const float* data) const;

    private:
        unsigned int handle_;

        int GetUniformLoc(const std::string& name) const;
    };
}

#endif //CGFIRE_SHADER_H
