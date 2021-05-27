//
// Created by JS on 25/05/2021.
//

#include "engine/Model.hpp"
#include "engine/Util.hpp"
#include "engine/config.hpp"

namespace en
{
    Model::Model(const std::string& path, bool flipUv)
    {
        flipUv_ = flipUv;

        std::string realPath = MODEL_ROOT + "/" + path;
        Log::Info("Loading Model " + realPath);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(realPath, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            Log::Error(std::string("Assimp Error - ") + importer.GetErrorString(), true);

        directory_ = realPath.substr(0, realPath.find_last_of('/'));

        Log::Info("Model has " + std::to_string(scene->mNumMeshes) + " meshes");
        LoadMaterials(scene);
        ProcessNode(scene->mRootNode, scene);
    }

    Model::~Model()
    {
        // TODO: delete textures and materials
    }

    void Model::Draw(const GLProgram* program) const
    {
        for (unsigned int i = 0; i < meshes_.size(); i++)
            meshes_[i].Draw(program);
    }

    void Model::LoadMaterials(const aiScene *scene)
    {
        if (!scene->HasMaterials())
            return;

        // Default Material
        /*materials_.insert(std::pair<const aiMaterial*, Material*>(nullptr,
                                                                  new Material(
                                                                          1.0f,
                                                                          glm::vec4(1.0f),
                                                                          glm::vec4(1.0f),
                                                                          nullptr)));*/

        unsigned int matCount = scene->mNumMaterials;
        Log::Info("Model has " + std::to_string(matCount) + " materials");
        for (unsigned int i = 0; i < matCount; i++)
        {
            // TODO: maybe load other texture types later
            aiMaterial* aiMat = scene->mMaterials[i];
            int diffuseTexCount = aiMat->GetTextureCount(aiTextureType_DIFFUSE);
            Log::Info("Material " + std::to_string(i) + " has " + std::to_string(diffuseTexCount) + " diffuse textures. Only loading first");

            // Shininess
            Material* material;
            float shininess;
            if (AI_SUCCESS != aiGetMaterialFloat(aiMat, AI_MATKEY_SHININESS, &shininess))
                shininess = 1.0f;
            Log::Info("Shininess " + std::to_string(shininess));

            // Diffuse color
            aiColor4D aiDiffuseColor;
            glm::vec4 diffuseColor;
            if (AI_SUCCESS != aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_DIFFUSE, &aiDiffuseColor))
                diffuseColor = glm::vec4(1.0f);
            else
                diffuseColor = glm::vec4(aiDiffuseColor.r, aiDiffuseColor.g, aiDiffuseColor.b, aiDiffuseColor.a);
            Log::Info(
                    "Diffuse color (" +
                    std::to_string(diffuseColor.x) +
                    ", " + std::to_string(diffuseColor.y) +
                    ", " + std::to_string(diffuseColor.z) +
                    ", " + std::to_string(diffuseColor.w) + ")");

            // Specular color
            aiColor4D aiSpecularColor;
            glm::vec4 specularColor;
            if (AI_SUCCESS != aiGetMaterialColor(aiMat, AI_MATKEY_COLOR_SPECULAR, &aiSpecularColor))
                specularColor = glm::vec4(1.0f);
            else
                specularColor = glm::vec4(aiSpecularColor.r, aiSpecularColor.g, aiSpecularColor.b, aiSpecularColor.a);
            Log::Info(
                    "Specular color (" +
                    std::to_string(specularColor.x) +
                    ", " + std::to_string(specularColor.y) +
                    ", " + std::to_string(specularColor.z) +
                    ", " + std::to_string(specularColor.w) + ")");

            // Diffuse texture
            GLPictureTex* texture = nullptr;
            if (diffuseTexCount > 0)
            {
                // TODO: maybe enable multiple textures later
                aiString fileName;
                aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &fileName);
                std::string filePath = directory_ + "/" + fileName.C_Str();
                if (textures_.find(filePath) == textures_.end())
                {
                    texture = new GLPictureTex(
                            GLPictureTex::WrapMode::CLAMP_TO_EDGE,
                            GLPictureTex::FilterMode::LINEAR,
                            filePath,
                            flipUv_);
                    textures_.insert(std::pair<std::string, GLPictureTex*>(filePath, texture));
                }
                else
                {
                    texture = textures_.at(filePath);
                }
            }

            // New material
            material = new Material(shininess, diffuseColor, specularColor, texture);
            materials_.insert(std::pair<const aiMaterial*, Material*>(aiMat, material));
        }
    }

    void Model::ProcessNode(aiNode* node, const aiScene* scene)
    {
        unsigned int meshCount = node->mNumMeshes;
        Log::Info("Node has " + std::to_string(meshCount) + " meshes");
        for (unsigned int i = 0; i < meshCount; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes_.push_back(ProcessMesh(mesh, scene));
        }

        unsigned int childCount = node->mNumChildren;
        Log::Info("Node has " + std::to_string(childCount) + " children");
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            ProcessNode(node->mChildren[i], scene);
    }

    Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<GLPictureTex> textures;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            glm::vec3 pos(0.0f);
            if (mesh->HasPositions())
            {
                pos.x = mesh->mVertices[i].x;
                pos.y = mesh->mVertices[i].y;
                pos.z = mesh->mVertices[i].z;
            }

            glm::vec3 normal(0.0f);
            if (mesh->HasNormals())
            {
                normal.x = mesh->mNormals[i].x;
                normal.y = mesh->mNormals[i].y;
                normal.z = mesh->mNormals[i].z;
            }

            glm::vec2 uv(0.0f);
            if (mesh->HasTextureCoords(0))
            {
                uv.x = mesh->mTextureCoords[0][i].x;
                uv.y = mesh->mTextureCoords[0][i].y;
            }

            Vertex vert(pos, normal, uv);
            vertices.push_back(vert);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        aiMaterial* aiRef = nullptr;
        if (mesh->mMaterialIndex >= 0)
            aiRef = scene->mMaterials[mesh->mMaterialIndex];
        Mesh m(vertices, indices, materials_.at(aiRef));
        return m;
    }
}
