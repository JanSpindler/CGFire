//
// Created by JS on 24/05/2021.
//

#ifndef CGFIRE_CONFIG_HPP
#define CGFIRE_CONFIG_HPP

#include <string>
#include <filesystem>

//#define SHADER_ROOT std::string("../shaders")
//#define MODEL_ROOT std::string("../models")
//#define DATA_ROOT std::string("../data")

#define ROOT_FOLDER std::filesystem::current_path().parent_path().parent_path().string()
#define MODEL_ROOT (ROOT_FOLDER + std::string("/models/"))
#define SHADER_ROOT (ROOT_FOLDER + std::string("/shaders/"))
#define DATA_ROOT (ROOT_FOLDER + std::string("/data/"))
//

#endif //CGFIRE_CONFIG_HPP
