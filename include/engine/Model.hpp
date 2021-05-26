//
// Created by JS on 25/05/2021.
//

#ifndef CGFIRE_MODEL_HPP
#define CGFIRE_MODEL_HPP

#include "Mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include "Material.hpp"
#include <unordered_map>

namespace en
{
    class Model
    {
    public:
        Model(const std::string& path, bool flipUv);
        ~Model();

        void Draw(const GLProgram* program) const;

    private:
        std::vector<Mesh> meshes_;
        std::string directory_;
        std::unordered_map<const aiMaterial*, Material*> materials_;
        std::unordered_map<std::string, GLPictureTex*> textures_;
        bool flipUv_;

        void LoadMaterials(const aiScene* scene);
        void ProcessNode(aiNode* node, const aiScene* scene);
        Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
    };
}

#endif //CGFIRE_MODEL_HPP
