//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/InitEvents.h"
#include "project/SceneEvent.h"
#include "project/ObjectManager.h"
#include "engine/render/SceneRenderer.hpp"


namespace scene {

    using namespace en;

    inline const char* RenderObjTypeNames[] = {"Sceletal", "Standard", "FixedColor", "Spline", "Reflective"};
    inline const int RenderObjTypeCount = 5;

    //Show an object or hide an object with this event. Also set the type of rendering
    class ShowRenderObjEvent : public Event {
    public:
        explicit ShowRenderObjEvent(en::SceneRenderer &sceneRenderer, scene::ObjectManager& objectManager)
        : m_SceneRenderer(sceneRenderer),
          m_ObjectManager(objectManager){}


          //This constructor is only here for automatic creation of this event when a RenderObj has been created.
          //So, this should only be used by EventManager in OnCreateNewObj()
        explicit ShowRenderObjEvent(SceneRenderer &sceneRenderer,
                                    ObjectManager& objectManager,
                                    RenderObj* obj,
                                    RenderObjType type,
                                    bool show)
                : m_SceneRenderer(sceneRenderer),
                  m_ObjectManager(objectManager),
                  m_Obj(obj),
                  m_Type(type),
                  m_Show(show){}


        EventType GetTypeID() override {return EventType::ShowRenderObjEvent; };

        Event* Clone() override{
            ShowRenderObjEvent* e = new ShowRenderObjEvent(m_SceneRenderer, m_ObjectManager);

            e->m_Obj = m_Obj;
            e->m_Type = m_Type;
            e->m_Show = m_Show;

            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: name RenderObjType show
            std::string name = data[0];
            m_Obj = m_ObjectManager.GetRenderObj(name);
            m_Type = static_cast<RenderObjType>(std::stoi(data[1]));
            m_Show = static_cast<bool>(std::stoi(data[2]));
            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_Obj->GetName() << static_cast<int>(m_Type) << static_cast<int>(m_Show);
        }

        void OnAction() override {
            switch (m_Type) {
                case RenderObjType::Skeletal:
                    if (m_Show)
                        m_SceneRenderer.AddSceletalRenderObj(dynamic_cast<Skeletal *>(m_Obj));
                    else
                        m_SceneRenderer.RemoveSceletalRenderObj(dynamic_cast<Skeletal *>(m_Obj));
                    break;
                case RenderObjType::Standard:
                    if (m_Show)
                        m_SceneRenderer.AddStandardRenderObj(m_Obj);
                    else
                        m_SceneRenderer.RemoveStandardRenderObj(m_Obj);
                    break;
                case RenderObjType::FixedColor:
                    if (m_Show)
                        m_SceneRenderer.AddFixedColorRenderObj(m_Obj);
                    else
                        m_SceneRenderer.RemoveFixedColorRenderObj(m_Obj);
                    break;
                case RenderObjType::Spline:
                    if (m_Show)
                        m_SceneRenderer.AddSplineRenderObj(m_Obj);
                    else
                        m_SceneRenderer.RemoveSplineRenderObj(m_Obj);
                    break;
                case RenderObjType::Reflective:
                    if (m_Show)
                        m_SceneRenderer.AddReflectiveRenderObj(m_Obj);
                    else
                        m_SceneRenderer.RemoveReflectiveRenderObj(m_Obj);
                    break;
            }
        }


        bool OnImGuiRender() override{
            static std::string s_ObjSelection;

            if (m_Obj !=  nullptr)
                s_ObjSelection = m_Obj->GetName();
            else
                s_ObjSelection = "";

            if (ImGui::BeginCombo("Object", s_ObjSelection.c_str()))
            {
                for (int i = 0; i < m_ObjectManager.GetAllRenderObjects().size(); ++i)
                {
                    auto& o = m_ObjectManager.GetAllRenderObjects()[i];
                    bool is_selected = (s_ObjSelection == o->GetName());
                    ImGui::PushID(i);
                    if (ImGui::Selectable(o->GetName().c_str(), is_selected)) {
                        s_ObjSelection = o->GetName();
                        m_Obj = o;
                        this->UpdateDescription();
                    }
                    ImGui::PopID();
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }


            if (m_Obj != nullptr) { // if it is nullptr, the dummy event is used to create the event no object has been set yet
                int s_TypeSelection = static_cast<int>(m_Type);

                if (ImGui::BeginCombo("Type", RenderObjTypeNames[s_TypeSelection])) {
                    for (int i = 0; i < RenderObjTypeCount; ++i) {
                        if (m_Obj->IsRenderObjTypePossible(static_cast<RenderObjType>(i))) {
                            bool is_selected = (s_TypeSelection == i);
                            ImGui::PushID(RenderObjTypeNames[i]);
                            if (ImGui::Selectable(RenderObjTypeNames[i], is_selected)) {
                                m_Type = static_cast<RenderObjType>(i);
                                this->UpdateDescription();
                            }
                            ImGui::PopID();
                            if (is_selected)
                                ImGui::SetItemDefaultFocus();
                        }
                    }
                    ImGui::EndCombo();
                }
            }

            if (ImGui::Checkbox("Show", &m_Show)){
                this->UpdateDescription();
            }

            bool optionsOk = m_Obj != nullptr && m_Obj->IsRenderObjTypePossible(m_Type);
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj* obj) override{
            if (obj == m_Obj)
                return true;
            return false;
        }

        void UpdateDescription() override{
            if (m_Obj != nullptr)
                m_Description = (m_Show ? "Shows object \'" : "Hides object \'")
                                + m_Obj->GetName()
                                + "\' of type \'" + RenderObjTypeNames[static_cast<int>(m_Type)] + "\'";
        }

    private:
        en::SceneRenderer &m_SceneRenderer;
        scene::ObjectManager& m_ObjectManager;
        RenderObj* m_Obj = nullptr;
        RenderObjType m_Type = RenderObjType::Standard;
        bool m_Show; //show or hide
    };
}