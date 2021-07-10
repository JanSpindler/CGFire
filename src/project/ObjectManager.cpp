//
//Created by vincent on 10.07.2021.
//

#include "project/ObjectManager.h"
#include "framework/imgui_util.hpp"


namespace scene {
    ObjectManager::ObjectManager() {

        std::vector<std::string> extensions = {".obj", ".dae", ".fbx"};
        util::GetAllFilesByExtensions(MODEL_ROOT, extensions, m_FoundObjectFiles);



        //Safety copy
        std::filesystem::copy_file(SCENE_DATA_ROOT + SceneObjectDataFileName,
                                   SCENE_DATA_ROOT + SceneObjectDataAutoCopyBeforeStartFileName,
                                   std::filesystem::copy_options::overwrite_existing);

        //Now read scene objects from file

        util::CSVReader csv(SCENE_DATA_ROOT + SceneObjectDataFileName);
        auto &table = csv.getTable();

        for (int i = 0; i < table.size(); ++i) {
            auto row = table[i];

            std::string idString = row[0];
            if (idString.empty() || idString[0] == '#') { //don't care about empty lines or comments
                continue;
            }


            en::RenderObj *renderObj = nullptr;

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
            } else if (id == ObjectType::Sceletal) {

            } else if (id == ObjectType::Spline) {
            } else {
                std::cout << "unknown object type-id:!!" << static_cast<int>(id) << std::endl;
                continue;
            }
            assert(renderObj != nullptr);
            m_AllRenderObjects.emplace_back(renderObj);
            renderObj->SetName(objName);
            //Now read RenderObjData: (Pos3f, RotationVector3f, RotationAngle1f, Scaling3f); it's stored in the next line
            auto RenderObjDataStr = table[i + 1];
            this->ReadRenderObjDataFromStrings(RenderObjDataStr, *renderObj);

            i++; //if we read an obj, we read two lines, so increase an extra one
        }

    }

    void ObjectManager::SaveToFile() {
        //first make a safe of the current scene before the loading. I
        // So, in case we made a mistake, we can still check out the old scene.
        std::filesystem::copy_file(SCENE_DATA_ROOT + SceneObjectDataFileName,
                                   SCENE_DATA_ROOT + SceneObjectDataAutoCopyLastFileName,
                                   std::filesystem::copy_options::overwrite_existing);

        //save the scene objects

        using namespace util;
        CSVWriter csv(SCENE_DATA_ROOT + SceneObjectDataFileName);

        for (auto &o : m_Models) {
            csv << static_cast<int>(ObjectType::Model) << o->GetName() << o->GetPathName() << endrow;
            SaveRenderObjDataToCSV(csv, *o);
        }
        for (auto &o : m_Sceletals) {
            //csv << static_cast<int>(ObjectType::Model) << o->GetName() << o->GetPathName();
            //SaveRenderObjDataToCSV(csv, *o);
        }
        for (auto &o : m_Splines) {
            //csv << static_cast<int>(ObjectType::Model) << o->GetName() << o->GetPathName();
            SaveRenderObjDataToCSV(csv, *o.second);
        }

    }


    void ObjectManager::OnImGuiRender() {
        ImGui::Begin("Objects");

        ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Model (non sceletal) ROs:");
        for (auto &obj : m_Models) {
            obj->OnImGuiRender();
        }
        OnImGuiAddModelRender();

        ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Sceletal ROs:");
        for (auto &obj : m_Sceletals) {
            obj->OnImGuiRender();
        }
        ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Spline ROs:");
        for (auto &obj : m_Splines) {
            obj.second->OnImGuiRender();
        }
        ImGui::End();
    }


    en::RenderObj* ObjectManager::GetRenderObj(const std::string &name) {
        for (auto &o : m_AllRenderObjects) {
            if (o->GetName() == name) {
                return o;
            }
        }
        std::cout << "Error: no such render obj: " << name << std::endl;
        return nullptr;
    }

    void ObjectManager::SaveRenderObjDataToCSV(util::CSVWriter &writer, en::RenderObj &o) {
        using namespace util;
        writer << o.Position.x << o.Position.y << o.Position.z
               << o.RotationAxis.x << o.RotationAxis.y << o.RotationAxis.z
               << o.RotationAngle
               << o.Scaling.x << o.Scaling.y << o.Scaling.z << endrow;
    }

    void ObjectManager::ReadRenderObjDataFromStrings(const std::vector <std::string> &str, en::RenderObj &o) {
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

    void ObjectManager::OnImGuiAddModelRender() {
        if (ImGui::Button("Add Model"))
            ImGui::OpenPopup("Add a Model");
        if (ImGui::BeginPopupModal("Add a Model", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

            static std::string s_FileSelection;

            if (ImGui::BeginCombo("Choose Model", s_FileSelection.c_str()))
            {
                for (auto& filePath : m_FoundObjectFiles)
                {
                    bool is_selected = (s_FileSelection == filePath);
                    if (ImGui::Selectable(filePath.c_str(), is_selected))
                        s_FileSelection = filePath;
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }


            static char s_Name[32] = "";
            ImGui::Text("Set name:");
            ImGui::InputText("##editmodel", s_Name, IM_ARRAYSIZE(s_Name));

            if (ImGui::Button("OK")) {

                bool isLegit = false;
                if (util::has_suffix(s_FileSelection, ".obj")
                    || util::has_suffix(s_FileSelection, ".dae")
                    || util::has_suffix(s_FileSelection, ".fbx")) {
                    isLegit = true;
                }

                if (std::strcmp(s_Name, "") == 0)
                    isLegit = false;

                if (isLegit) {
                    //Add model
                    auto model = std::make_shared<en::Model>(s_FileSelection, true);
                    model->SetName(s_Name);
                    m_Models.emplace_back(model);
                    m_AllRenderObjects.emplace_back(model.get());

                    ImGui::CloseCurrentPopup();
                } else {
                    ImGui::OpenPopup("Error");
                }
            }

            bool unused_open = true;
            if (ImGui::BeginPopupModal("Error", &unused_open)) {
                ImGui::Text("Something is wrong. File needs to be .obj/.dae/.fbx");
                if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            ImGui::SameLine();
            if (ImGui::Button("Abort")) {
                strcpy_s(s_Name, "");
                s_FileSelection = "";
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }
}