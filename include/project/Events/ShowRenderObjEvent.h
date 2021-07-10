//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
#include "project/ObjectManager.h"
#include "engine/render/SceneRenderer.hpp"

using namespace particle;

namespace scene {

    using namespace en;

    inline const char* RenderObjTypeNames[] = {"Sceletal", "Standard", "FixedColor", "Spline", "Reflective"};
    inline const int RenderObjTypeCount = 5;

    class ShowRenderObjEvent : public Event {
    public:
        EventType GetTypeID() override {return EventType::ShowRenderObjEvent; };

        explicit ShowRenderObjEvent(SceneRenderer &sceneRenderer, scene::ObjectManager& objectManager)
                : m_SceneRenderer(sceneRenderer),
                  m_ObjectManager(objectManager) {}

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: name RenderObjType
            std::string name = data[0];
            m_Obj = m_ObjectManager.GetRenderObj(name);
            m_Type = static_cast<RenderObjType>(std::stoi(data[1]));
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_Obj->GetName() << static_cast<int>(m_Type);
        }

        void OnAction() override {
            switch (m_Type) {
                case RenderObjType::Sceletal:
                    m_SceneRenderer.AddSceletalRenderObj(dynamic_cast<Sceletal *>(m_Obj));
                    break;
                case RenderObjType::Standard:
                    m_SceneRenderer.AddStandardRenderObj(m_Obj);
                    break;
                case RenderObjType::FixedColor:
                    m_SceneRenderer.AddFixedColorRenderObj(m_Obj);
                    break;
                case RenderObjType::Spline:
                    m_SceneRenderer.AddSplineRenderObj(m_Obj);
                    break;
                case RenderObjType::Reflective:
                    m_SceneRenderer.AddReflectiveRenderObj(m_Obj);
                    break;
            }
        }

        void OnImGuiRender() override{
            OnImGuiRenderSetOptions();
            m_Obj->OnImGuiRender();
        }

        bool OnImGuiRenderSetOptions() override{
            static std::string s_ObjSelection;

            if (m_Obj !=  nullptr)
                s_ObjSelection = m_Obj->GetName();
            else
                s_ObjSelection = "";

            if (ImGui::BeginCombo("Object", s_ObjSelection.c_str()))
            {
                for (auto& o : m_ObjectManager.GetAllRenderObjects())
                {
                    bool is_selected = (s_ObjSelection == o->GetName());
                    if (ImGui::Selectable(o->GetName().c_str(), is_selected)) {
                        s_ObjSelection = o->GetName();
                        m_Obj = o;
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }


            int s_TypeSelection = static_cast<int>(m_Type);

            if (ImGui::BeginCombo("Type", RenderObjTypeNames[s_TypeSelection]))
            {
                for (int i = 0; i < RenderObjTypeCount; ++i)
                {
                    if (m_Obj->IsRenderObjTypePossible(static_cast<RenderObjType>(i))) {
                        bool is_selected = (s_TypeSelection == i);
                        if (ImGui::Selectable(RenderObjTypeNames[i], is_selected)) {
                            m_Type = static_cast<RenderObjType>(i);
                        }
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }

            bool optionsOk = m_Obj != nullptr && m_Obj->IsRenderObjTypePossible(m_Type);
            return optionsOk;
        }

    private:
        SceneRenderer &m_SceneRenderer;
        scene::ObjectManager& m_ObjectManager;
        RenderObj* m_Obj = nullptr;
        RenderObjType m_Type = RenderObjType::Standard;
    };
}