//
// Created by JS on 25/05/2021.
//

#include "engine/Model.hpp"
#include "engine/util.hpp"
#include "engine/config.hpp"

namespace en
{
    Model::Model(const std::string& path, bool flipUv)
    {
        flipUv_ = flipUv;
        LoadModel(MODEL_ROOT + "/" + path);
    }

    Model::~Model()
    {
    }

    void Model::Draw(const GLProgram* program) const
    {
        for (unsigned int i = 0; i < meshes_.size(); i++)
            meshes_[i].Draw(program);
    }

    void Model::LoadModel(const std::string& path)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            Log::Error(std::string("Assimp Error - ") + importer.GetErrorString(), true);

        directory = path.substr(0, path.find_last_of('/'));
        ProcessNode(scene->mRootNode, scene);
    }

    void Model::ProcessNode(aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes_.push_back(ProcessMesh(mesh, scene));
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
            ProcessNode(node->mChildren[i], scene);
    }

    Mesh Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

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

            Vertex vert = { pos, normal, uv };
            vertices.push_back(vert);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        if (mesh->mMaterialIndex >= 0)
        {
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

            std::vector<Texture> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        }

        Mesh m(vertices, indices, textures);
        return m;
    }

    std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName)
    {
        std::vector<Texture> textures;
        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            bool skip = false;
            for(unsigned int j = 0; j < loadedTextures_.size(); j++)
            {
                if(std::strcmp(loadedTextures_[j].path.data(), str.C_Str()) == 0)
                {
                    textures.push_back(loadedTextures_[j]);
                    skip = true;
                    break;
                }
            }
            if(!skip)
            {   // if texture hasn't been loaded already, load it
                Texture texture(std::string(directory + "/" + str.C_Str()), flipUv_);
                texture.type = typeName;
                texture.path = str.C_Str();
                textures.push_back(texture);
                loadedTextures_.push_back(texture); // add to loaded textures
            }
        }
        return textures;
    }
}
