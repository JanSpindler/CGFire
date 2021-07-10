//
//Created by vincent on 08.07.2021.
//

#pragma once
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "engine/config.hpp"
#include "util/CSVReader.h"
#include "util/CSVWriter.h"
#include "util/StringUtil.h"
#include "util/FileFinder.h"
#include "framework/imgui_util.hpp"
#include "engine/model/Model.hpp"
#include "engine/Sceletal.hpp"
#include "engine/Spline3D.hpp"


namespace scene {
    class ObjectManager {
        const std::string SceneObjectDataFileName = "sceneobjects.csv";
        const std::string SceneObjectDataAutoCopyLastFileName = "autocopy_sceneobjects.csv";
        const std::string SceneObjectDataAutoCopyBeforeStartFileName = "autocopylaststart_sceneobjects.csv";
    public:
        enum class ObjectType {
            Model = 0,
            Sceletal = 1,
            Spline = 2
            };

        ObjectManager();

        void SaveToFile();


        void OnImGuiRender();


        en::RenderObj* GetRenderObj(const std::string& name);

        std::vector<en::RenderObj*>& GetAllRenderObjects(){ return m_AllRenderObjects; };
        std::vector<std::shared_ptr<en::Model>>& GetModels(){ return m_Models; }
        std::vector<std::shared_ptr<en::Sceletal>>& GetSceletals(){ return m_Sceletals; }
        std::vector<std::pair<std::shared_ptr<en::Spline3D>, std::shared_ptr<en::Spline3DRenderable>>>& GetSplines(){ return m_Splines; }
    private:
        std::vector<en::RenderObj*> m_AllRenderObjects;

        std::vector<std::shared_ptr<en::Model>> m_Models; //Those objects that are models but have no animation
        std::vector<std::shared_ptr<en::Sceletal>> m_Sceletals; //Those objects that are models and have an animation
        std::vector<std::pair<std::shared_ptr<en::Spline3D>, std::shared_ptr<en::Spline3DRenderable>>> m_Splines;

        static void SaveRenderObjDataToCSV(util::CSVWriter& writer, en::RenderObj& o);
        static void ReadRenderObjDataFromStrings(const std::vector<std::string>& str, en::RenderObj& o);

        void OnImGuiAddModelRender();


        std::vector<std::string> m_FoundObjectFiles; // stores all paths to object files in /models/ directory
    };
}