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

        ObjectManager(){
            //Safety copy
            std::filesystem::copy_file(SCENE_DATA_ROOT + SceneObjectDataFileName, SCENE_DATA_ROOT + SceneObjectDataAutoCopyBeforeStartFileName,
                                       std::filesystem::copy_options::overwrite_existing);

            //Now read scene objects from file

            util::CSVReader csv(SCENE_DATA_ROOT + SceneObjectDataFileName);
            auto& table = csv.getTable();

            for (int i = 0; i < table.size(); ++i){
                auto row = table[i];

                std::string idString = row[0];
                if (idString.empty() || idString[0] == '#' ){ //don't care about empty lines or comments
                    continue;
                }


                en::RenderObj* renderObj = nullptr;

                //layout, saved in two lines:
                // id name <SpecificStuff>
                // <RenderObjData>

                std::string objName = row[1];
                //Read SpecificStuff
                ObjectType id = static_cast<ObjectType>(std::stoi(idString));
                if (id == ObjectType::Model) {
                        // SpecificStuff: modelfile
                        std::string file = row[2];
                        auto model = std::make_shared<en::Model>(file, true);
                        m_Models.emplace_back(model);
                        renderObj = model.get();
                }
                else if (id == ObjectType::Sceletal) {

                }
                else if (id == ObjectType::Spline) {
                }
                else{
                    std::cout << "unknown object type-id:!!" << static_cast<int>(id) << std::endl;
                    continue;
                }
                assert(renderObj != nullptr);
                renderObj->SetName(objName);
                //Now read RenderObjData: (Pos3f, RotationVector3f, RotationAngle1f, Scaling3f); it's stored in the next line
                auto RenderObjDataStr = table[i+1];
                this->ReadRenderObjDataFromStrings(RenderObjDataStr, *renderObj);

                i++; //if we read an obj, we read two lines, so increase an extra one
            }

        }

        void SaveToFile(){
            //first make a safe of the current scene before the loading. I
            // So, in case we made a mistake, we can still check out the old scene.
            std::filesystem::copy_file(SCENE_DATA_ROOT + SceneObjectDataFileName, SCENE_DATA_ROOT + SceneObjectDataAutoCopyLastFileName,
                                       std::filesystem::copy_options::overwrite_existing);

            //save the scene objects

            using namespace util;
            CSVWriter csv(SCENE_DATA_ROOT + SceneObjectDataFileName);

            for (auto& o : m_Models){
                csv << static_cast<int>(ObjectType::Model) << o->GetName() << o->GetPathName() << endrow;
                SaveRenderObjDataToCSV(csv, *o);
            }
            for (auto& o : m_Sceletals){
                //csv << static_cast<int>(ObjectType::Model) << o->GetName() << o->GetPathName();
                //SaveRenderObjDataToCSV(csv, *o);
            }
            for (auto& o : m_Splines){
                //csv << static_cast<int>(ObjectType::Model) << o->GetName() << o->GetPathName();
                SaveRenderObjDataToCSV(csv, *o.second);
            }

        }


        //Folgende Methoden sollen sowohl über UI als auch wegen der eingelesenen Datei benutzt werden

        void AddModel(const std::string& file, const std::string& name){

        }
        void AddSceletal(){

        }
        void AddSpline(){

        }
        //etc...

        void OnImGuiRender(){
            ImGui::Begin("Objects");

            ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Model (non sceletal) ROs:");
            for (auto& obj : m_Models) {
                obj->OnImGuiRender();
            }
            ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Sceletal ROs:");
            for (auto& obj : m_Sceletals) {
                obj->OnImGuiRender();
            }
            ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Spline ROs:");
            for (auto& obj : m_Splines) {
                obj.second->OnImGuiRender();
            }
            ImGui::End();
        }


        std::shared_ptr<en::RenderObj> GetRenderObj(const std::string& name){
            for (auto& m : m_Models){
                if (m->GetName() == name){
                    return m;
                }
            }
            for (auto& m : m_Sceletals){
                if (m->GetName() == name){
                    return m;
                }
            }
            for (auto& m : m_Splines){
                if (m.second->GetName() == name){
                    return m.second;
                }
            }
            std::cout << "Error: no such render obj: " << name << std::endl;
            return nullptr;
        }

    private:
        std::vector<std::shared_ptr<en::Model>> m_Models; //Those objects that are models but have no animation
        std::vector<std::shared_ptr<en::Sceletal>> m_Sceletals; //Those objects that are models and have an animation
        std::vector<std::pair<std::shared_ptr<en::Spline3D>, std::shared_ptr<en::Spline3DRenderable>>> m_Splines;

        void SaveRenderObjDataToCSV(util::CSVWriter& writer, en::RenderObj& o){
            using namespace util;
            writer  << o.Position.x << o.Position.y << o.Position.z
                    << o.RotationAxis.x << o.RotationAxis.y << o.RotationAxis.z
                    << o.RotationAngle
                    << o.Scaling.x << o.Scaling.y << o.Scaling.z << endrow;
        }
        void ReadRenderObjDataFromStrings(const std::vector<std::string>& str, en::RenderObj& o){
            o.Position.x = std::stof(str[0]);
            o.Position.y = std::stof(str[1]);
            o.Position.z = std::stof(str[2]);
            o.RotationAxis.x = std::stof(str[3]);
            o.RotationAxis.y = std::stof(str[4]);
            o.RotationAxis.z = std::stof(str[5]);
            o.RotationAngle = std::stof(str[6]);
            o.Scaling.x = std::stof(str[7]);
            o.Scaling.y = std::stof(str[8]);
            o.Scaling.z = std::stof(str[9]);
        }
    };
}