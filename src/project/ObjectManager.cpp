//
//Created by vincent on 10.07.2021.
//

#include "project/ObjectManager.h"
#include "framework/imgui_util.hpp"
#include "project/EventManager.h" // when objects are deleted, the EventManager is informed
#include "project/SceneManager.h"

namespace scene {
    ObjectManager::ObjectManager(SceneManager& sceneManager, EventManager& eventManager)
    : m_SceneManager(sceneManager),
    m_EventManager(eventManager){

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

                renderObj = this->LoadModel(objName, file).get();
            } else if (id == ObjectType::Sceletal) {

            } else if (id == ObjectType::Spline) {
            } else {
                std::cout << "unknown object type-id:!!" << static_cast<int>(id) << std::endl;
                continue;
            }
            assert(renderObj != nullptr);

            //renderObj->SetName(objName);//already in LoadModel, comment out for now
            //m_AllRenderObjects.emplace_back(renderObj);//already in LoadModel, comment out for now

            //Now read RenderObjData: (Pos3f, RotationVector3f, RotationAngle1f, Scaling3f); it's stored in the next line
            auto RenderObjDataStr = table[i + 1];
            this->ReadRenderObjDataFromStrings(RenderObjDataStr, *renderObj);

            i++; //if we read an obj, we read two lines, so increase an extra one
        }

    }

    void ObjectManager::SaveToFile() {
        //save the scene objects

        using namespace util;
        CSVWriter csv(SCENE_DATA_ROOT + SceneObjectDataFileName);

        for (auto &o : m_Models) {
            csv << static_cast<int>(ObjectType::Model) << o->GetName() << o->GetPathName() << endrow;
            SaveRenderObjDataToCSV(csv, *o);
        }
        for (auto &o : m_Sceletals) {

        }
        for (auto &o : m_Splines) {

        }

    }

    ObjectManager::ButtonClickHappened ObjectManager::OnImGuiObjectRender(en::RenderObj& o){
        auto cloneOrDeleteHappened = ButtonClickHappened::None;
        //std::string strID = "RO " + std::to_string(o.ID) + ": " + o.GetName();
        ImGui::PushID(o.ID);

        bool isOpen = ImGui::TreeNode(o.GetName().c_str());
        ImGui::SameLine();
        if (ImGui::Button("Clone")){
            //Clone
            cloneOrDeleteHappened = ButtonClickHappened::Clone;
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete")){
            ImGui::OpenPopup("Are you sure?");
        }
        if (ImGui::BeginPopupModal("Are you sure?", nullptr)) {
            ImGui::Text("Delete %s?", o.GetName().c_str());
            if (ImGui::Button("No"))
                ImGui::CloseCurrentPopup();
            if (ImGui::Button("Yes")) {
                cloneOrDeleteHappened = ButtonClickHappened::Delete;
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::SameLine();
        static char s_RenameString[32] = "";
        if (ImGui::Button("Rename")){
            strcpy_s(s_RenameString,  o.GetName().c_str());
            ImGui::OpenPopup("Rename Object");
        }
        if (ImGui::BeginPopupModal("Rename Object", nullptr)) {

            ImGui::Text("Choose Name:", o.GetName().c_str());
            ImGui::InputText("##editobjname", s_RenameString, IM_ARRAYSIZE(s_RenameString));


            if (ImGui::Button("Apply")) {
                if (!this->DoesObjNameExistAlready(s_RenameString)) {
                    o.SetName(s_RenameString);
                    m_EventManager.UpdateEventDescriptions();
                    ImGui::CloseCurrentPopup();
                }
            }
            ImGui::SameLine();
            if (ImGui::Button("Abort"))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }

        if (isOpen) {
            ImGui::DragFloat3("Position", &o.Position.x, 0.25f);
            ImGui::DragFloat("RotationAngle", &o.RotationAngle, 0.1f, 0.f, 6.28318530718f);
            ImGui::DragFloat3("RotationAxisVector", &o.RotationAxis.x, 0.05f, 0.f, 1.f);
            ImGui::DragFloat3("Scaling", &o.Scaling.x, 0.25f, 0.f, 999.f);

            if (ImGui::TreeNode("Events")) {
                m_EventManager.OnImGuiRenderEventsOfObj(&o);
                ImGui::TreePop();
            }

            ImGui::TreePop();
        }
        ImGui::PopID();
        return cloneOrDeleteHappened;
    }

    void DeleteRenderObjFromVec(RenderObj* ro, std::vector<RenderObj*>& vec){
        for (auto it = vec.begin(); it != vec.end(); it++){
            if (ro->GetName() == (*it)->GetName()){
                vec.erase(it);
                break;
            }
        }
    }
    void ObjectManager::OnImGuiRender() {
        ImGui::Begin("Objects");

        ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Model (non sceletal) ROs:");
        for (auto it = m_Models.begin(); it != m_Models.end(); ++it) {
            en::Model* o = it->get();
            auto cloneOrDeleteHappened = OnImGuiObjectRender(*o);
            if (cloneOrDeleteHappened == ButtonClickHappened::Delete){
                //Remove Model from scene
                m_EventManager.OnObjDeletion(o);
                DeleteRenderObjFromVec(o, m_AllRenderObjects); //important to do this before erasing from models
                m_Models.erase(it);

                m_SceneManager.restart(false);
                break;
            }
            else if (cloneOrDeleteHappened == ButtonClickHappened::Clone){
                //Clone Model
                auto model = this->LoadModel(this->FindNextAvailableName(o->GetName()), o->GetPathName());
                assert(model != nullptr);
                m_SceneManager.restart(false);
                break;
            }
        }
        OnImGuiAddObjectRender(ObjectType::Model);

        ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Sceletal ROs:");
        for (auto &obj : m_Sceletals) {
            //OnImGuiObjectRender(*obj);
        }
        OnImGuiAddObjectRender(ObjectType::Sceletal);

        ImGui::TextColored(ImVec4(1.f, 0.f, 1.f, 1.f), "Spline ROs:");
        for (auto &obj : m_Splines) {
            //OnImGuiObjectRender(*obj.second);
        }
        OnImGuiAddObjectRender(ObjectType::Spline);

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

    void ObjectManager::OnImGuiAddObjectRender(ObjectType type) {
        char addStr[16] = "";
        switch (type) {
            case ObjectType::Model:
                strcpy_s(addStr, "Add Model");
                break;
            case ObjectType::Spline:
                strcpy_s(addStr, "Add Spline");
                break;
            case ObjectType::Sceletal:
                strcpy_s(addStr, "Add Sceletal");
                break;
        }


        if (ImGui::Button(addStr))
            ImGui::OpenPopup(addStr);
        if (ImGui::BeginPopupModal(addStr, nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {

            static char s_Name[32] = "";
            ImGui::Text("Set name:");
            ImGui::SameLine();
            ImGui::InputText("##editname", s_Name, IM_ARRAYSIZE(s_Name));


            //Render Settings and OK button
            switch (type) {
                case ObjectType::Model:
                    OnImGuiAddModelObjRender(s_Name);
                    break;
                case ObjectType::Spline:
                    break;
                case ObjectType::Sceletal:
                    break;
            }

            ImGui::SameLine();
            if (ImGui::Button("Abort")) {
                strcpy_s(s_Name, "");
                ImGui::CloseCurrentPopup();
            }


            if (ImGui::BeginPopupModal("Error")) {
                ImGui::Text("Something is wrong. Files need to be .obj/.dae/.fbx.\n"
                            "Also no name duplicates or empty names!");
                if (ImGui::Button("Close"))
                    ImGui::CloseCurrentPopup();
                ImGui::EndPopup();
            }

            ImGui::EndPopup();
        }
    }

    void ObjectManager::OnImGuiAddModelObjRender(const std::string& name){
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


        if (ImGui::Button("OK")) {

            bool isLegit = false;
            if (util::HasSuffix(s_FileSelection, ".obj")
                || util::HasSuffix(s_FileSelection, ".dae")
                || util::HasSuffix(s_FileSelection, ".fbx")) {
                isLegit = true;
            }

            if (name == "")
                isLegit = false;

            if (isLegit) {
                //Add new model
                auto model = this->LoadModel(name, s_FileSelection);
                if (model != nullptr) {
                    SaveToFile();
                    m_EventManager.OnCreateNewObj(model.get()); //Creates by default an ShowRenderObjEvent event

                    ImGui::CloseCurrentPopup();
                }
                else{
                    ImGui::OpenPopup("Error");
                }

            } else {
                ImGui::OpenPopup("Error");
            }
        }
    }

    bool ObjectManager::DoesObjNameExistAlready(const std::string& name){
        for (auto& m : m_AllRenderObjects){ //check if name already exists
            if (m->GetName() == name){
                return true;
            }
        }
        return false;
    }
    std::shared_ptr<en::Model> ObjectManager::LoadModel(const std::string& name, const std::string& file){
        if (DoesObjNameExistAlready(name))
            return nullptr;
        auto model = std::make_shared<en::Model>(file, true);
        model->SetName(name);
        m_Models.emplace_back(model);
        m_AllRenderObjects.emplace_back(model.get());
        return model;
    }

    std::string ObjectManager::FindNextAvailableName(const std::string& name){
        std::string newName = name;
        int k = 0;
        while (this->DoesObjNameExistAlready(newName)){
            newName = name + std::to_string(k);
            k++;
        }
        return newName;
    }
}