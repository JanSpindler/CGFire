//
// Created by JS on 23/05/2021.
//

#include "engine/gr_include.hpp"
#include "engine/Render/GLShader.hpp"
#include "engine/Util.hpp"
#include "engine/config.hpp"

namespace en
{
    std::unordered_map<std::string, GLShader*> GLShader::instances_ = {};

    const GLShader* GLShader::Load(const std::string &filename)
    {
        GLShader* shader;
        if (instances_.find(filename) == instances_.end())
            instances_[filename] = new GLShader(filename);
        return instances_[filename];
    }

    void GLShader::Delete(const GLShader* shader)
    {
        GLShader* storedShader;
        for (std::unordered_map<std::string, GLShader*>::const_iterator i = instances_.begin(); i != instances_.end(); i++)
        {
            storedShader = i->second;
            if (storedShader == shader)
            {
                delete storedShader;
                instances_.erase(i);
                return;
            }
        }
    }

    void GLShader::DeleteAll()
    {
        for (std::unordered_map<std::string, GLShader*>::const_iterator i = instances_.begin(); i != instances_.end(); i++)
            delete i->second;
        instances_.clear();
    }

    GLShader::GLShader(const std::string& filename)
    {
        Log::Info("Loading shader code: " + filename);

        std::vector<char> code = ReadFileBinary(SHADER_ROOT + "/" + filename.c_str());
        code.push_back(0);

        uint32_t glType = 0;
        if (filename.find(".vert") != std::string::npos)
            glType = GL_VERTEX_SHADER;
        else if (filename.find(".geom") != std::string::npos)
            glType = GL_GEOMETRY_SHADER;
        else if (filename.find(".frag") != std::string::npos)
            glType = GL_FRAGMENT_SHADER;
        else
            Log::Error("Failed to find corresponding GL Shader Type", true);

        handle_ = glCreateShader(glType);
        const char* c_code = code.data();
        glShaderSource(handle_, 1, &c_code, nullptr);

        Log::Info("Compiling shader code");

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
        Delete();
    }

    void GLShader::AttachTo(unsigned int program) const
    {
        glAttachShader(program, handle_);
    }

    void GLShader::Delete()
    {
        glDeleteShader(handle_);
    }

    std::unordered_map<GLProgram::ShaderStages, GLProgram*> GLProgram::instances_ = {};

    const GLProgram* GLProgram::Load(const GLShader *vert, const GLShader *geom, const GLShader *frag)
    {
        ShaderStages stages = { vert, geom, frag };
        if (instances_.find(stages) == instances_.end())
            instances_[stages] = new GLProgram(vert, geom, frag);
        return instances_[stages];
    }

    void GLProgram::Delete(const GLProgram* program)
    {
        GLProgram* storedProgram;
        for (std::unordered_map<ShaderStages, GLProgram*>::const_iterator i = instances_.begin(); i != instances_.end(); i++)
        {
            storedProgram = i->second;
            if (storedProgram == program)
            {
                delete storedProgram;
                instances_.erase(i);
                return;
            }
        }
    }

    void GLProgram::DeleteAll()
    {
        for (std::unordered_map<ShaderStages, GLProgram*>::const_iterator i = instances_.begin(); i != instances_.end(); i++)
            delete i->second;
        instances_.clear();
    }

    GLProgram::GLProgram(const GLShader* vertShader, const GLShader* geomShader, const GLShader* fragShader)
    {
        if (vertShader == nullptr || fragShader == nullptr)
            Log::Error("GLProgram needs at least a vertex and a fragment Shader", true);

        handle_ = glCreateProgram();
        vertShader->AttachTo(handle_);
        fragShader->AttachTo(handle_);
        if (geomShader != nullptr)
            geomShader->AttachTo(handle_);
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
        Delete();
    }

    void GLProgram::Use() const
    {
        glUseProgram(handle_);
    }

    void GLProgram::Delete() const
    {
        glDeleteProgram(handle_);
    }

    void GLProgram::SetUniformF(const std::string& name, float val) const
    {
        glUniform1f(GetUniformLoc(name), val);
    }

    void GLProgram::SetUniformI(const std::string& name, int val) const
    {
        glUniform1i(GetUniformLoc(name), val);
    }

    void GLProgram::SetUniformUI(const std::string& name, unsigned int val) const
    {
        glUniform1ui(GetUniformLoc(name), val);
    }

    void GLProgram::SetUniformB(const std::string& name, bool val) const
    {
        glUniform1i(GetUniformLoc(name), static_cast<int>(val));
    }

    void GLProgram::SetUniformVec2f(const std::string& name, const glm::vec2& val) const
    {
        glUniform2f(GetUniformLoc(name), val.x, val.y);
    }

    void GLProgram::SetUniformVec3f(const std::string& name, const glm::vec3& val) const
    {
        glUniform3f(GetUniformLoc(name), val.x, val.y, val.z);
    }

    void GLProgram::SetUniformVec4f(const std::string& name, const glm::vec4& val) const
    {
        glUniform4f(GetUniformLoc(name), val.x, val.y, val.z, val.w);
    }

    void GLProgram::SetUniformMat2(const std::string& name, bool transpose, const float* data) const
    {
        glUniformMatrix2fv(GetUniformLoc(name), 1, transpose, data);
    }

    void GLProgram::SetUniformMat3(const std::string& name, bool transpose, const float* data) const
    {
        glUniformMatrix3fv(GetUniformLoc(name), 1, transpose, data);
    }

    void GLProgram::SetUniformMat4(const std::string& name, bool transpose, const float* data) const
    {
        glUniformMatrix4fv(GetUniformLoc(name), 1, transpose, data);
    }

    int GLProgram::GetUniformLoc(const std::string& name) const
    {
        return (int) glGetUniformLocation(handle_, name.c_str());
    }
}
