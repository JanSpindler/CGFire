//
// Created by JS on 23/05/2021.
//

#include "engine/gr_include.hpp"

#include "engine/GLShader.hpp"
#include "engine/Util.hpp"
#include "engine/config.hpp"

namespace en
{
    GLShader::GLShader(const std::string& filename, Type type)
    {
        std::vector<char> code = ReadFileBinary(SHADER_ROOT + "/" + filename.c_str());
        code.push_back(0);

        unsigned int glType;
        switch (type)
        {
            case Type::VERTEX:
                glType = GL_VERTEX_SHADER;
                break;
            case Type::FRAGMENT:
                glType = GL_FRAGMENT_SHADER;
                break;
            default:
                Log::Error("Unknown Shader type", true);
        }

        handle_ = glCreateShader(glType);
        const char* c_code = code.data();
        glShaderSource(handle_, 1, &c_code, nullptr);
        glCompileShader(handle_);

        int  success;
        char infoLog[512];
        glGetShaderiv(handle_, GL_COMPILE_STATUS, &success);
        if(!success)
        {
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
        glLinkProgram(handle_);

        int success;
        char infoLog[512];
        glGetProgramiv(handle_, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(handle_, 512, nullptr, infoLog);
            Log::Info(infoLog);
            Log::Error("Failed to link GLProgram", true);
        }
    }

    GLProgram::~GLProgram()
    {
    }

    void GLProgram::Use() const
    {
        glUseProgram(handle_);
    }

    void GLProgram::Delete() const
    {
        glDeleteProgram(handle_);
    }

    void GLProgram::SetUniformF(const char* name, float val) const
    {
        glUniform1f(GetUniformLoc(name), val);
    }

    void GLProgram::SetUniformI(const char* name, int val) const
    {
        glUniform1i(GetUniformLoc(name), val);
    }

    void GLProgram::SetUniformUI(const char* name, unsigned int val) const
    {
        glUniform1ui(GetUniformLoc(name), val);
    }

    void GLProgram::SetUniformB(const char* name, bool val) const
    {
        glUniform1i(GetUniformLoc(name), static_cast<int>(val));
    }

    void GLProgram::SetUniformVec2f(const char* name, const glm::vec2& val) const
    {
        glUniform2f(GetUniformLoc(name), val.x, val.y);
    }

    void GLProgram::SetUniformVec3f(const char* name, const glm::vec3& val) const
    {
        glUniform3f(GetUniformLoc(name), val.x, val.y, val.z);
    }

    void GLProgram::SetUniformVec4f(const char* name, const glm::vec4& val) const
    {
        glUniform4f(GetUniformLoc(name), val.x, val.y, val.z, val.w);
    }

    void GLProgram::SetUniformMat2(const char* name, bool transpose, const float* data) const
    {
        glUniformMatrix2fv(GetUniformLoc(name), 1, transpose, data);
    }

    void GLProgram::SetUniformMat3(const char* name, bool transpose, const float* data) const
    {
        glUniformMatrix3fv(GetUniformLoc(name), 1, transpose, data);
    }

    void GLProgram::SetUniformMat4(const char* name, bool transpose, const float* data) const
    {
        glUniformMatrix4fv(GetUniformLoc(name), 1, transpose, data);
    }

    int GLProgram::GetUniformLoc(const char *name) const
    {
        return (int) glGetUniformLocation(handle_, name);
    }
}
