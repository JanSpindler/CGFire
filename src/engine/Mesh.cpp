//
// Created by JS on 23/05/2021.
//

#include "engine/gr_include.hpp"

//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <glm/gtx/hash.hpp>

#include "engine/Mesh.hpp"
#include "engine/util.hpp"

namespace en
{
    Texture::Texture(const std::string &path)
    {
        Log::Info(path);

        int width;
        int height;
        int channelCount;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channelCount, 0);
        if (!data)
            Log::Error("Failed to load Texture", true);
        Log::Info("Channel count " + std::to_string(channelCount));

        glGenTextures(1, &id);
        glBindTexture(GL_TEXTURE_2D, id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);
    }

    Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        vertices_ = vertices;
        indices_ = indices;
        textures_ = textures;
        SetupMesh();
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::Draw(const GLProgram* program) const
    {
        unsigned int diffuseNr = 0;
        unsigned int specularNr = 0;
        for (unsigned int i = 0; i < textures_.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            std::string number;
            std::string name = textures_[i].type;
            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);

            program->SetUniformI((name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, textures_[i].id);
        }
        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(vao_);
        glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void Mesh::SetupMesh()
    {
        glGenVertexArrays(1, &vao_);
        glBindVertexArray(vao_);

        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), vertices_.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &ibo_);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), indices_.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::pos));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::normal));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, Vertex::uv));

        glBindVertexArray(0);
    }
}
