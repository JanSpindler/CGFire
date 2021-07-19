//
//Created by vincent on 10.07.2021.
//

#pragma once
#include <filesystem>

namespace util{
    namespace fs = std::filesystem;

    //original from https://stackoverflow.com/a/11142540
    inline void GetAllFilesByExtension(fs::path const & root, std::string const & ext, std::vector<std::string>& files)
    {
        if (fs::exists(root) && fs::is_directory(root))
        {
            for (auto const & entry : fs::recursive_directory_iterator(root))
            {
                if (fs::is_regular_file(entry) && entry.path().extension() == ext)
                    files.emplace_back(entry.path().string().substr(root.string().size()));
            }
        }
    }

    inline void GetAllFilesByExtensions(fs::path const & root, std::vector<std::string> extensions,  std::vector<std::string>& files)
    {
        for (auto& ext : extensions){
            GetAllFilesByExtension(root, ext, files);
        }
    }
}