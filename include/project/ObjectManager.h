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
#include "engine/model/Model.hpp"
#include "engine/Sceletal.hpp"
#include "engine/Spline3D.hpp"

namespace scene {


    class ObjectManager {
    public:
        enum class ObjectTypes {
            Model = 0,
            Sceletal = 1,
            Spline = 2
            };

        ObjectManager(){
            //read scene objects from file

            util::CSVReader csv(SCENE_DATA_ROOT + "sceneobjects.csv");
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

                //Read SpecificStuff
                std::string objName = row[1];
                ObjectTypes id = static_cast<ObjectTypes>(std::stoi(idString));
                if (id == ObjectTypes::Model) {
                        // SpecificStuff: modelfile
                        std::string file = row[2];
                        auto model = std::make_shared<en::Model>(file, true);
                        m_Models.emplace_back(model);
                        renderObj = model.get();
                }
                else if (id == ObjectTypes::Sceletal) {

                }
                else if (id == ObjectTypes::Spline) {
                }
                else{
                    std::cout << "unknown object type-id:!!" << static_cast<int>(id) << std::endl;
                    continue;
                }

                assert(renderObj != nullptr);
                renderObj->SetName(objName);
                //Now read RenderObjData: (Pos3f, RotationVector3f, RotationAngle1f, Scaling3f), saved in the next line
                auto RenderObjDataStr = table[i+1];
                renderObj->Position.x = std::stof(RenderObjDataStr[0]);
                renderObj->Position.y = std::stof(RenderObjDataStr[1]);
                renderObj->Position.z = std::stof(RenderObjDataStr[2]);
                renderObj->RotationAxis.x = std::stof(RenderObjDataStr[3]);
                renderObj->RotationAxis.y = std::stof(RenderObjDataStr[4]);
                renderObj->RotationAxis.z = std::stof(RenderObjDataStr[5]);
                renderObj->RotationAngle = std::stof(RenderObjDataStr[6]);
                renderObj->Scaling.x = std::stof(RenderObjDataStr[7]);
                renderObj->Scaling.y = std::stof(RenderObjDataStr[8]);
                renderObj->Scaling.z = std::stof(RenderObjDataStr[9]);


                i++; //if we read an obj, we read two lines, so increase an extra one
            }

        }

        void SaveToFile(){
            //save the scene
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
        std::vector<std::shared_ptr<en::Model>> m_Models;
        std::vector<std::shared_ptr<en::Sceletal>> m_Sceletals;
        std::vector<std::pair<std::shared_ptr<en::Spline3D>, std::shared_ptr<en::Spline3DRenderable>>> m_Splines;
    };
}