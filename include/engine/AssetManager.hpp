//
// Created by JS on 25/05/2021.
//

#ifndef CGFIRE_ASSETMANAGER_HPP
#define CGFIRE_ASSETMANAGER_HPP

#include <string>

#include "Mesh.hpp"

namespace en
{
    class AssetManager
    {
    public:
        static Mesh LoadMesh(const std::string& filename);

    private:
    };
}

#endif //CGFIRE_ASSETMANAGER_HPP
