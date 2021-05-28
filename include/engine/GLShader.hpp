//
// Created by JS on 23/05/2021.
//

#ifndef CGFIRE_SHADER_H
#define CGFIRE_SHADER_H

#include <glm/glm.hpp>

namespace en
{
    class GLShader
    {
    public:
        GLShader(const char* filename, unsigned int type);
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

        void Link() const;
        void Delete() const;

        void SetUniform1f(const char* name, float val) const;
        void SetUniform1i(const char* name, int val) const;
        void SetUniform1ui(const char* name, unsigned int val) const;
        void SetUniform1b(const char* name, bool val) const;

        void SetUniform2fv(const char* name, const glm::vec2& val) const;
        void SetUniform3fv(const char* name, const glm::vec3& val) const;
        void SetUniform4fv(const char* name, const glm::vec4& val) const;

        void SetUniformMat2(const char* name, bool transpose, const glm::mat2& val) const;
        void SetUniformMat3(const char* name, bool transpose, const glm::mat3& val) const;
        void SetUniformMat4(const char* name, bool transpose, const glm::mat4& val) const;

    private:
        unsigned int handle_;

        int GetUniformLoc(const char* name) const;
    };
}

#endif //CGFIRE_SHADER_H
