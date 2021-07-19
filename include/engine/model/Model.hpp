//
// Created by JS on 25/05/2021.
//
//Annika added skeletal animation support 29/05/21, based on https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation

#ifndef CGFIRE_MODEL_HPP
#define CGFIRE_MODEL_HPP

#include "Mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "engine/render/Material.hpp"
#include <unordered_map>
#include <map>
#include "engine/render/Renderable.hpp"

namespace en
{
    struct boneinfo{
        int boneid;
        glm::mat4 offsetmat;
    };

    class Model : public RenderObj
    {
    public:
        Model(const std::string& path, bool flipUv, bool subtransform, const std::string& name);
        ~Model();

        void Render(const GLProgram* program) override;
        void RenderPosOnly(const GLProgram* program) override;
        void RenderDiffuse(const GLProgram* program) override;
        void RenderAll(const GLProgram* program) override;

        uint32_t GetMeshCount() const;
        const Material* GetMeshMaterial(uint32_t meshIndex) const;
        void SetMeshMaterial(uint32_t meshIndex, const Material* material);

        std::map<std::string, boneinfo> getbonemap();
        int getbonecount() const;
        const std::vector<Mesh*>& GetMeshes() const;


        const std::string& GetPathName() const;
        bool IsRenderObjTypePossible(RenderObjType type) const override;

    private:
        std::vector<Mesh*> meshes_;
        std::string pathName_;
        std::string directory_;
        std::unordered_map<const aiMaterial*, Material*> materials_;
        std::unordered_map<std::string, GLPictureTex*> textures_;
        bool flipUv_;
        std::map<std::string, boneinfo> bonemap;
        int bonecount = 0;

        void LoadMaterials(const aiScene* scene);
        void ProcessNode(aiNode* node, const aiScene* scene, glm::mat4 parentT, bool subtransform);
        Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene, glm::mat4 t);
        void BoneweightforVertices(std::vector<Vertex>& vertices, aiMesh* mesh);
    };
}

#endif //CGFIRE_MODEL_HPP
