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

        void SetUniformF(const char* name, float val) const;
        void SetUniformI(const char* name, int val) const;
        void SetUniformUI(const char* name, unsigned int val) const;
        void SetUniformB(const char* name, bool val) const;

        void SetUniformVec2f(const char* name, const glm::vec2& val) const;
        void SetUniformVec3f(const char* name, const glm::vec3& val) const;
        void SetUniformVec4f(const char* name, const glm::vec4& val) const;

        void SetUniformMat2(const char* name, bool transpose, const float* data) const;
        void SetUniformMat3(const char* name, bool transpose, const float* data) const;
        void SetUniformMat4(const char* name, bool transpose, const float* data) const;

    private:
        unsigned int handle_;

        int GetUniformLoc(const char* name) const;
    };
}

#endif //CGFIRE_SHADER_H
