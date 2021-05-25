//
// Created by JS on 25/05/2021.
//

#ifndef CGFIRE_MODEL_HPP
#define CGFIRE_MODEL_HPP

#include "Mesh.hpp"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace en
{
    class Model
    {
    public:
        Model(const std::string& path);
        ~Model();

        void Draw(const GLProgram* program) const;

    private:
        std::vector<Mesh> meshes_;
        std::string directory;
        std::vector<Texture> loadedTextures_;

        void LoadModel(const std::string& path);
        void ProcessNode(aiNode* node, const aiScene* scene);
        Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::string& typeName);
    };
}

#endif //CGFIRE_MODEL_HPP
