//
// Created by JS on 23/05/2021.
//

#include "engine/gr_include.hpp"

#include "engine/GLShader.hpp"
#include "engine/Util.hpp"

namespace en
{
    GLShader::GLShader(const char* filename, unsigned int type)
    {
        std::vector<char> code = ReadFileBinary(filename);

        handle_ = glCreateShader(type);
        const char* c_code = code.data();
        glShaderSource(handle_, 1, &c_code, nullptr);
        glCompileShader(handle_);

        int  success;
        char infoLog[512];
        glGetShaderiv(handle_, GL_COMPILE_STATUS, &success);
        if(!success) {
            glGetShaderInfoLog(handle_, 512, NULL, infoLog);
            Log::Info(infoLog);
            Log::Error("Failed to compile GLShader", true);
        }
    }

    GLShader::~GLShader()
    {
    }

    void GLShader::AttachTo(unsigned int program) const
    {
        glAttachShader(program, handle_);
    }

    void GLShader::Delete() const
    {
        glDeleteShader(handle_);
    }

    GLProgram::GLProgram(const GLShader& vertShader, const GLShader& fragShader)
    {
        handle_ = glCreateProgram();
        vertShader.AttachTo(handle_);
        fragShader.AttachTo(handle_);
        Link();

        int success;
        char infoLog[512];
        glGetProgramiv(handle_, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(handle_, 512, NULL, infoLog);
            Log::Info(infoLog);
            Log::Error("Failed to link GLProgram", true);
        }
    }

    GLProgram::~GLProgram()
    {
    }

    void GLProgram::Link() const
    {
        glLinkProgram(handle_);
    }

    void GLProgram::Delete() const
    {
        glDeleteProgram(handle_);
    }

    void GLProgram::SetUniform1f(const char* name, float val) const
    {
        glUniform1f(GetUniformLoc(name), val);
    }

    void GLProgram::SetUniform1i(const char* name, int val) const
    {
        glUniform1i(GetUniformLoc(name), val);
    }

    void GLProgram::SetUniform1ui(const char* name, unsigned int val) const
    {
        glUniform1ui(GetUniformLoc(name), val);
    }

    void GLProgram::SetUniform1b(const char* name, bool val) const
    {
        glUniform1i(GetUniformLoc(name), static_cast<int>(val));
    }

    void GLProgram::SetUniform2fv(const char* name, const glm::vec2& val) const
    {
        glUniform2fv(GetUniformLoc(name), 1, &val[0]);
    }

    void GLProgram::SetUniform3fv(const char* name, const glm::vec3& val) const
    {
        glUniform3fv(GetUniformLoc(name), 1, &val[0]);
    }

    void GLProgram::SetUniform4fv(const char* name, const glm::vec4& val) const
    {
        glUniform4fv(GetUniformLoc(name), 1, &val[0]);
    }

    void GLProgram::SetUniformMat2(const char* name, bool transpose, const glm::mat2& val) const
    {
        glUniformMatrix2fv(GetUniformLoc(name), 1, transpose, &val[0][0]);
    }

    void GLProgram::SetUniformMat3(const char* name, bool transpose, const glm::mat3& val) const
    {
        glUniformMatrix3fv(GetUniformLoc(name), 1, transpose, &val[0][0]);
    }

    void GLProgram::SetUniformMat4(const char* name, bool transpose, const glm::mat4& val) const
    {
        glUniformMatrix4fv(GetUniformLoc(name), 1, transpose, &val[0][0]);
    }

    int GLProgram::GetUniformLoc(const char *name) const
    {
        return (int) glGetUniformLocation(handle_, name);
    }
}
