//
// Created by JS on 23/05/2021.
//

#ifndef CGFIRE_SHADER_H
#define CGFIRE_SHADER_H

#include <glm/glm.hpp>
#include <string>
#include "GLTexture.hpp"
#include <unordered_map>

namespace en
{
    class GLShader
    {
    public:
        static const GLShader* Load(const std::string& filename);
        static void Delete(const GLShader* shader);
        static void DeleteAll();

        ~GLShader();

        void AttachTo(unsigned int program) const;
        void Delete();

    private:
        static std::unordered_map<std::string, GLShader*> instances_;

        unsigned int handle_;

        GLShader(const std::string& filename);
    };

    class GLProgram
    {
    public:
        struct ShaderStages {
            const GLShader* vert;
            const GLShader* geom;
            const GLShader* frag;

            bool operator ==(const ShaderStages& other) const
            {
                return (vert == other.vert) && (geom == other.geom) && (frag == other.frag);
            }
        };

        static const GLProgram* Load(const GLShader* vert, const GLShader* geom, const GLShader* frag);
        static void Delete(const GLProgram*);
        static void DeleteAll();

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
        static std::unordered_map<ShaderStages, GLProgram*> instances_;

        unsigned int handle_;

        GLProgram(const GLShader* vertShader, const GLShader* geomShader, const GLShader* fragShader);

        int GetUniformLoc(const std::string& name) const;
    };
}

namespace std
{
    template<>
    struct hash<en::GLProgram::ShaderStages>
    {
        std::size_t operator()(const en::GLProgram::ShaderStages& k) const
        {
            uint64_t v1 = (uint64_t)k.vert;
            uint64_t v2 = (uint64_t)k.geom;
            uint64_t v3 = (uint64_t)k.frag;
            return v1 ^ v2 ^ v3;
        }
    };
}

#endif //CGFIRE_SHADER_H
