//
// Created by JS on 25/05/2021.
//
//Annika added skeletal animation support 29/05/21, based on https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#include "engine/Model/Model.hpp"
#include "engine/Util.hpp"
#include "engine/config.hpp"
#include "util/assimptoglmmatrix.h"
#include <glm/gtx/string_cast.hpp>

namespace en
{
    Model::Model(const std::string& path, bool flipUv, bool subtransform)
    : pathName_(path)
    {
        flipUv_ = flipUv;

        std::string realPath = MODEL_ROOT + "/" + path;
        Log::Info("Loading model " + realPath);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(realPath, aiProcess_Triangulate | aiProcess_FlipUVs);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
            Log::Error(std::string("Assimp Error - ") + importer.GetErrorString(), true);

        directory_ = realPath.substr(0, std::max(realPath.find_last_of('/'), realPath.find_last_of('\\')));

        Log::Info("model has " + std::to_string(scene->mNumMeshes) + " meshes");
        LoadMaterials(scene);
        ProcessNode(scene->mRootNode, scene, glm::identity<glm::mat4>(), subtransform);
    }

    Model::~Model()
    {
        for (Mesh* mesh : meshes_)
            delete mesh;

        for (const std::pair<const aiMaterial*, Material*>& pair : materials_)
            delete pair.second;

        for (const std::pair<std::string, GLPictureTex*>& pair : textures_)
            delete pair.second;
    }

    void Model::Render(const GLProgram *program) // Deprecated
    {
        RenderObj::Render(program);
        for (Mesh* mesh : meshes_)
            mesh->Render(program);
    }

    void Model::RenderPosOnly(const GLProgram *program)
    {
        RenderObj::RenderPosOnly(program);
        for (Mesh* mesh : meshes_)
            mesh->RenderPosOnly(program);
    }

    void Model::RenderDiffuse(const GLProgram *program)
    {
        RenderObj::RenderDiffuse(program);
        for (Mesh* mesh : meshes_)
            mesh->RenderDiffuse(program);
    }

    void Model::RenderAll(const GLProgram *program)
    {
        RenderObj::RenderAll(program);
        for (Mesh* mesh : meshes_)
            mesh->RenderAll(program);
    }

    uint32_t Model::GetMeshCount() const
    {
        return meshes_.size();
    }

    const Material* Model::GetMeshMaterial(uint32_t meshIndex) const
    {
        if (meshIndex >= meshes_.size())
            Log::Error("Mesh index exceeding boundaries", true);
        return meshes_[meshIndex]->GetMaterial();
    }

    void Model::SetMeshMaterial(uint32_t meshIndex, const Material* material)
    {
        if (meshIndex >= meshes_.size())
            Log::Error("Mesh index exceeding boundaries", true);
        meshes_[meshIndex]->SetMaterial(material);
    }

    const std::vector<Mesh*>& Model::GetMeshes() const
    {
        return meshes_;
    }

    const std::string& Model::GetPathName() const{
        return pathName_;
    }

    bool Model::IsRenderObjTypePossible(en::RenderObjType type) const{
        switch(type){
            case RenderObjType::Standard:
            case RenderObjType::FixedColor:
            case RenderObjType::Reflective:
                return true;
            default:
                return false;
        }
    }

    void Model::LoadMaterials(const aiScene *scene)
    {
        if (!scene->HasMaterials())
            return;

        unsigned int matCount = scene->mNumMaterials;
        Log::Info("model has " + std::to_string(matCount) + " materials");
        for (unsigned int i = 0; i < matCount; i++)
        {
            // TODO: maybe load other texture types later
            aiMaterial* aiMat = scene->mMaterials[i];
            int diffuseTexCount = aiMat->GetTextureCount(aiTextureType_DIFFUSE);
            int specularTexCount = aiMat->GetTextureCount(aiTextureType_SPECULAR);
            Log::Info("Material " + std::to_string(i) + " has " + std::to_string(diffuseTexCount) + " diffuse textures. Only loading first");
            Log::Info("Material " + std::to_string(i) + " has " + std::to_string(specularTexCount) + " specular textures. Only loading first");

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
            GLPictureTex* diffuseTex = nullptr;
            if (diffuseTexCount > 0)
            {
                // TODO: maybe enable multiple textures later
                aiString fileName;
                aiMat->GetTexture(aiTextureType_DIFFUSE, 0, &fileName);
                std::string filePath = directory_ + "/" + fileName.C_Str();
                if (textures_.find(filePath) == textures_.end())
                {
                    diffuseTex = new GLPictureTex(
                            GLPictureTex::WrapMode::CLAMP_TO_EDGE,
                            GLPictureTex::FilterMode::LINEAR,
                            filePath,
                            flipUv_);
                    textures_.insert(std::pair<std::string, GLPictureTex*>(filePath, diffuseTex));
                }
                else
                {
                    diffuseTex = textures_.at(filePath);
                }
            }

            // Specular texture
            GLPictureTex* specularTex = nullptr;
            if (specularTexCount > 0)
            {
                // TODO: maybe enable multiple textures later
                aiString fileName;
                aiMat->GetTexture(aiTextureType_SPECULAR, 0, &fileName);
                std::string filePath = directory_ + "/" + fileName.C_Str();
                if (textures_.find(filePath) == textures_.end())
                {
                    specularTex = new GLPictureTex(
                            GLPictureTex::WrapMode::CLAMP_TO_EDGE,
                            GLPictureTex::FilterMode::LINEAR,
                            filePath,
                            flipUv_);
                    textures_.insert(std::pair<std::string, GLPictureTex*>(filePath, specularTex));
                }
                else
                {
                    specularTex = textures_.at(filePath);
                }
            }

            // New material
            material = new Material(diffuseTex, specularTex, diffuseColor, specularColor, shininess);
            materials_.insert(std::pair<const aiMaterial*, Material*>(aiMat, material));
        }
    }

    void Model::ProcessNode(aiNode* node, const aiScene* scene, glm::mat4 parentT, bool subtransform)
    {
        glm::mat4 totalT;
        if (subtransform)
        {
            aiMatrix4x4 localAiT = node->mTransformation;
            glm::mat4 localT (
                    localAiT.a1, localAiT.b1, localAiT.c1, localAiT.d1,
                    localAiT.a2, localAiT.b2, localAiT.c2, localAiT.d2,
                    localAiT.a3, localAiT.b3, localAiT.c3, localAiT.d3,
                    localAiT.a4, localAiT.b4, localAiT.c4, localAiT.d4
            );
            totalT = parentT * localT;
        }
        else
        {
            totalT = glm::identity<glm::mat4>();
        }


        unsigned int meshCount = node->mNumMeshes;
        Log::Info("Node has " + std::to_string(meshCount) + " meshes");
        for (unsigned int i = 0; i < meshCount; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes_.push_back(ProcessMesh(mesh, scene, totalT));
        }

        unsigned int childCount = node->mNumChildren;
        Log::Info("Node has " + std::to_string(childCount) + " children");
        for (unsigned int i = 0; i < node->mNumChildren; i++)
            ProcessNode(node->mChildren[i], scene, totalT, subtransform);
    }

    std::map<std::string, boneinfo>& Model::getbonemap() {
        return bonemap;
    }
    int Model::getbonecount() const { return bonecount;}
    void Model::BoneweightforVertices(std::vector<Vertex>& vertices, aiMesh* mesh){
        for (int i = 0; i < mesh->mNumBones; i++){
            std::string bonename = mesh->mBones[i]->mName.C_Str();
            int id = -1;
            if(bonemap.find(bonename) == bonemap.end()) {
                boneinfo newinfo{};
                newinfo.boneid = bonecount;
                newinfo.offsetmat = util::AssimptoGLM4x4(mesh->mBones[i]->mOffsetMatrix);
                bonemap.insert(std::pair<std::string, boneinfo>(bonename, newinfo));
                id = bonecount;
                bonecount++;
                //Log::Info("added offsetmatrix to"+bonename+glm::to_string(newinfo.offsetmat));
            }
            else{
                id = bonemap.find(bonename)->second.boneid;
            }
            aiVertexWeight* boneweights = mesh->mBones[i]->mWeights;
            int num = mesh->mBones[i]->mNumWeights;
            for (int k=0;k<num;k++){
                assert(boneweights[k].mVertexId<=vertices.size());
                for (int l = 0; l<MAXBONEINFLUENCE; l++){
                    if(vertices[boneweights[k].mVertexId].boneids_[l]<=0) {
                        vertices[boneweights[k].mVertexId].boneids_[l] = id;
                        vertices[boneweights[k].mVertexId].boneweights_[l] = boneweights[k].mWeight;
                        //Log::Info("Extracted boneweight" + std::to_string(vertices[boneweights[k].mVertexId].boneweights_[l]));
                        break;
                    }
                }
            }


        }

    }

    Mesh* Model::ProcessMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 t)
    {
        glm::mat3 normalMat = glm::mat3(glm::transpose(glm::inverse(t)));

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

            pos = glm::vec3(t * glm::vec4(pos, 1.0f));
            normal = normalMat * normal;

            Vertex vert(pos, normal, uv);
            vertices.push_back(vert);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace& face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++)
                indices.push_back(face.mIndices[j]);
        }

        aiMaterial* aiRef = nullptr;
        if (mesh->mMaterialIndex >= 0)
            aiRef = scene->mMaterials[mesh->mMaterialIndex];
        if (mesh->HasBones()){BoneweightforVertices(vertices, mesh);}
        Mesh* m = new Mesh(vertices, indices, materials_.at(aiRef));
        return m;
    }
}
