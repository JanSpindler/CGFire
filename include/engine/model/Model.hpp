//
// Created by JS on 25/05/2021.
//

#ifndef CGFIRE_MODEL_HPP
#define CGFIRE_MODEL_HPP

#include "Mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "engine/Render/Material.hpp"
#include <unordered_map>
#include "engine/Render/Renderable.hpp"

namespace en
{
    class Model : public Renderable
    {
    public:
        Model(const std::string& path, bool flipUv);
        ~Model();

        void Render(const GLProgram* program) const override;
        void RenderGeometry(const GLProgram* program) const override;

        const std::vector<Mesh*>& GetMeshes() const;

    private:
        std::vector<Mesh*> meshes_;
        std::string directory_;
        std::unordered_map<const aiMaterial*, Material*> materials_;
        std::unordered_map<std::string, GLPictureTex*> textures_;
        bool flipUv_;

        void LoadMaterials(const aiScene* scene);
        void ProcessNode(aiNode* node, const aiScene* scene);
        Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
    };
}

#endif //CGFIRE_MODEL_HPP
