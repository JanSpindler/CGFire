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
#include "engine/Skeletal.hpp"
#include "engine/Spline3D.hpp"



namespace scene {
    class EventManager;
    class SceneManager;

    typedef std::pair<std::shared_ptr<en::Spline3D>, std::shared_ptr<en::Spline3DRenderable>> tSpline;


    enum class ObjectType {
        Model = 0,
        Skeletal = 1,
        Spline = 2
    };

    class ObjectManager {
        const std::string SceneObjectDataFileName = "sceneobjects.csv"; //this should always be the most recent edit
        const std::string SceneObjectDataAutoCopyBeforeStartFileName = "autocopylaststart_sceneobjects.csv";
    public:

        explicit ObjectManager(SceneManager& sceneManager, EventManager& eventManager);

        void SaveToFile();


        void OnImGuiRender();
        void OnRecalculateSplines();

        en::RenderObj* GetRenderObj(const std::string& name);

        std::vector<en::RenderObj*>& GetAllRenderObjects(){ return m_AllRenderObjects; };
        std::vector<std::shared_ptr<en::Model>>& GetModels(){ return m_Models; }
        std::vector<std::shared_ptr<en::Skeletal>>& GetSceletals(){ return m_Skeletals; }
        std::vector<std::pair<std::shared_ptr<en::Spline3D>, std::shared_ptr<en::Spline3DRenderable>>>& GetSplines(){ return m_Splines; }
        std::string GetNameOfSpline(en::Spline3D* spline);
        std::shared_ptr<en::Spline3D> GetSplineByName(const std::string name);

        bool AreSplinesDrawn();
    private:
        SceneManager& m_SceneManager; //TODO probably we should simply create a static instance of each manager
        EventManager& m_EventManager;
        std::vector<en::RenderObj*> m_AllRenderObjects;

        std::vector<std::shared_ptr<en::Model>> m_Models; //Those objects that are models but have no animation
        std::vector<std::shared_ptr<en::Skeletal>> m_Skeletals; //Those objects that are models and have an animation
        std::vector<tSpline> m_Splines;

        std::vector<std::string> m_FoundObjectFiles; // stores all paths to object files in /models/ directory
        std::vector<std::string> m_FoundDaeFiles; // stores all paths to Animated object files in /models/ directory

        static void SaveRenderObjDataToCSV(util::CSVWriter& writer, en::RenderObj& o);
        static void ReadRenderObjDataFromStrings(const std::vector<std::string>& str, en::RenderObj& o);

        void OnImGuiAddObjectRender(ObjectType type);

        void OnImGuiAddModelObjRender(const std::string& name); //used by OnImGuiAddObjectRender

        void OnImGuiAddSkeletalObjRender(const std::string& name); //used by OnImGuiAddObjectRender
        void OnImGuiAddSkeletalAnimationButtonRender(std::string& animationCustomName, std::string& animationFileSelection);
        static void OnImGuiSkeletalAnimationsListRender(std::vector<std::pair<std::string, std::string>>& animationNamesAndFilePaths);

        void OnImGuiAddSplineObjRender(const std::string& name);
        void OnImGuiSplineSettingsRender();

        bool DoesObjNameExistAlready(const std::string& name);
        std::string FindNextAvailableName(const std::string& name);

        std::shared_ptr<en::Model> LoadModel(const std::string& name, const std::string& file);
        std::shared_ptr<en::Skeletal> LoadSkeletal(const std::string& name, const std::string& file,
                                                   std::vector<std::pair<std::string, std::string>>& animationNamesAndFilePaths);
        tSpline LoadSpline(const std::string& name,
                           bool loop,
                           uint32_t resolution,
                           uint8_t type,
                           const std::vector<glm::vec3>& controlPoints);

        enum class ButtonClickHappened {None, Clone, Delete}; //There may be a better approach to this but idk
        ButtonClickHappened ObjectManager::OnImGuiObjectRender(ObjectType type, en::RenderObj& o);

    };
}