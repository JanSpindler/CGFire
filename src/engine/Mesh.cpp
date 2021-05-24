//
// Created by JS on 23/05/2021.
//

#include "engine/gr_include.hpp"

#include <functional>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/gtc/type_ptr.hpp>

#include "engine/Mesh.hpp"
#include "engine/config.hpp"

namespace en
{
    std::vector<Mesh> Mesh::LoadScene(const char* filename, bool smooth, glm::vec4 preColor)
    {
        Assimp::Importer importer;
        int process = aiProcess_Triangulate | aiProcess_JoinIdenticalVertices;
        if (smooth) {
            process |= aiProcess_GenSmoothNormals;
        } else {
            process |= aiProcess_GenNormals;
        }
        const aiScene* scene = importer.ReadFile(MODEL_ROOT + "/" + filename, process);
        if (scene == nullptr) return {};

        std::vector<glm::vec4> colors;
        for (unsigned int i = 0; i < scene->mNumMaterials; ++i) {
            aiMaterial* mat = scene->mMaterials[i];
            aiColor3D color(1.f, 1.f, 1.f);
            mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
            colors.push_back(glm::vec4(color[0], color[1], color[2], 1.f));
        }

        std::vector<Mesh> objects;
        std::function<void(aiNode*, glm::mat4)> traverse;
        traverse = [&](aiNode* node, glm::mat4 t) {
            aiMatrix4x4 aim = node->mTransformation;
            glm::mat4 new_t(
                    aim.a1, aim.b1, aim.c1, aim.d1,
                    aim.a2, aim.b2, aim.c2, aim.d2,
                    aim.a3, aim.b3, aim.c3, aim.d3,
                    aim.a4, aim.b4, aim.c4, aim.d4
            );
            t = new_t * t;

            aiMatrix4x4 local_t;
            if (node->mNumMeshes > 0) {
                for (uint32_t i = 0; i < node->mNumMeshes; ++i) {
                    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

                    Mesh m;
                    m.positions_.resize(mesh->mNumVertices);
                    m.normals_.resize(mesh->mNumVertices);
                    m.colors_.resize(mesh->mNumVertices, scene->mNumMaterials ? colors[mesh->mMaterialIndex] : preColor);
                    m.faces_.resize(mesh->mNumFaces);

                    float* vbo_data = new float[mesh->mNumVertices * 10];
                    unsigned int* ibo_data = new unsigned int[mesh->mNumFaces * 3];
                    for (uint32_t i = 0; i < mesh->mNumVertices; ++i) {
                        glm::vec3 pos(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
                        glm::vec3 nrm(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
                        glm::vec4 col = m.colors_[i];
                        if (mesh->HasVertexColors(0)) {
                            col[0] = mesh->mColors[0][i].r;
                            col[1] = mesh->mColors[0][i].g;
                            col[2] = mesh->mColors[0][i].b;
                            col[3] = mesh->mColors[0][i].a;
                        }

                        m.positions_[i] = pos;
                        m.normals_[i] = nrm;
                        m.colors_[i] = col;

                        vbo_data[10 * i + 0] = pos[0];
                        vbo_data[10 * i + 1] = pos[1];
                        vbo_data[10 * i + 2] = pos[2];
                        vbo_data[10 * i + 3] = nrm[0];
                        vbo_data[10 * i + 4] = nrm[1];
                        vbo_data[10 * i + 5] = nrm[2];
                        vbo_data[10 * i + 6] = col[0];
                        vbo_data[10 * i + 7] = col[1];
                        vbo_data[10 * i + 8] = col[2];
                        vbo_data[10 * i + 9] = col[3];
                    }

                    for (uint32_t i = 0; i < mesh->mNumFaces; ++i) {
                        glm::uvec3 face(mesh->mFaces[i].mIndices[0],
                                        mesh->mFaces[i].mIndices[1],
                                        mesh->mFaces[i].mIndices[2]);
                        m.faces_[i] = face;
                        ibo_data[i * 3 + 0] = face[0];
                        ibo_data[i * 3 + 1] = face[1];
                        ibo_data[i * 3 + 2] = face[2];
                    }

                    glGenVertexArrays(1, &m.vao_);
                    glBindVertexArray(m.vao_);

                    m.vbo_ = GLBuffer(GL_ARRAY_BUFFER, GL_STATIC_DRAW, mesh->mNumVertices * 10 * sizeof(float), vbo_data);
                    m.ibo_ = GLBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW, mesh->mNumFaces * 3 * sizeof(unsigned int), ibo_data);

                    m.vbo_.Bind();

                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)0);
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(3*sizeof(float)));
                    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(float), (void*)(6*sizeof(float)));
                    glEnableVertexAttribArray(0);
                    glEnableVertexAttribArray(1);
                    glEnableVertexAttribArray(2);

                    m.ibo_.Bind();

                    m.transform_ = t;
                    m.vertexCount_ = 3 * mesh->mNumFaces;

                    objects.push_back(m);

                    delete [] vbo_data;
                    delete [] ibo_data;
                }
            }

            for (uint32_t i = 0; i < node->mNumChildren; ++i) {
                traverse(node->mChildren[i], t);
            }
        };

        traverse(scene->mRootNode, glm::identity<glm::mat4>());
        return objects;
    }

    Mesh::Mesh(const char* filename, bool smooth, glm::vec4 preColor)
    {
        Mesh m = LoadScene(filename, smooth, preColor)[0];
        vao_ = m.vao_;
        vbo_ = m.vbo_;
        ibo_ = m.ibo_;
        transform_ = m.transform_;
        vertexCount_ = m.vertexCount_;
        positions_ = m.positions_;
        normals_ = m.normals_;
        colors_ = m.colors_;
        faces_ = m.faces_;
    }

    Mesh::~Mesh()
    {
    }

    void Mesh::Bind() const
    {
        glBindVertexArray(vao_);
    }

    void Mesh::Release() const
    {
        glBindVertexArray(0);
    }

    void Mesh::Delete() const
    {
        Release();
        glDeleteVertexArrays(1, &vao_);
        vbo_.Delete();
        ibo_.Delete();
    }

    Mesh::Mesh()
    {
    }
}
