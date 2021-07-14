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

    //Play the animation of a skeletal
    class PlayAnimationEvent : public Event {
    public:
        explicit PlayAnimationEvent(en::SceneRenderer &sceneRenderer, scene::ObjectManager& objectManager)
        : m_SceneRenderer(sceneRenderer),
          m_ObjectManager(objectManager){}


        EventType GetTypeID() override {return EventType::PlayAnimationEvent; };

        Event* Clone() override{
            PlayAnimationEvent* e = new PlayAnimationEvent(m_SceneRenderer, m_ObjectManager);

            e->m_Obj = m_Obj;
            e->m_AnimName = m_AnimName;

            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: objName animationName
            std::string objName = data[0];
            std::string animationName = data[1];
            m_Obj = dynamic_cast<en::Skeletal*>(m_ObjectManager.GetRenderObj(objName));
            m_AnimName = animationName;

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_Obj->GetName() << m_AnimName;
        }

        void OnAction() override {
            m_Obj->PlayAnimation(m_AnimName);
        }


        bool OnImGuiRender() override{
            static std::string s_ObjSelection;

            if (m_Obj !=  nullptr)
                s_ObjSelection = m_Obj->GetName();
            else
                s_ObjSelection = "";

            if (ImGui::BeginCombo("Object", s_ObjSelection.c_str()))
            {
                auto& allSkeletals = m_ObjectManager.GetSceletals();
                for (int i = 0; i < allSkeletals.size(); ++i)
                {
                    auto& o = allSkeletals[i];
                    bool is_selected = (s_ObjSelection == o->GetName());
                    ImGui::PushID(i);
                    if (ImGui::Selectable(o->GetName().c_str(), is_selected)) {
                        s_ObjSelection = o->GetName();
                        if (m_Obj != o.get())
                            m_AnimName = "";
                        m_Obj = o.get();
                        this->UpdateDescription();
                    }
                    ImGui::PopID();
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }


            if (m_Obj != nullptr) { // if it is nullptr, the dummy event is used to create the event, so no object has been set yet

                if (ImGui::BeginCombo("Animation", m_AnimName.c_str())) {
                    auto& animations = m_Obj->GetAnimationNamesAndFiles();
                    for (int i = 0; i < animations.size(); ++i){
                        bool is_selected = (m_AnimName == animations[i].first);

                        ImGui::PushID(i);
                        if (ImGui::Selectable(animations[i].first.c_str(), is_selected)) {
                            m_AnimName = animations[i].first;
                            this->UpdateDescription();
                        }
                        ImGui::PopID();
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                    ImGui::EndCombo();
                }
            }

            bool optionsOk = m_Obj != nullptr && !m_AnimName.empty();
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj* obj) override{
            if (obj == m_Obj)
                return true;
            return false;
        }

        void UpdateDescription() override{
            if (m_Obj != nullptr)
                m_Description = "Plays animation \'"
                        + m_AnimName
                        + "\' of object "
                        + m_Obj->GetName();
        }

    private:
        en::SceneRenderer &m_SceneRenderer;
        scene::ObjectManager& m_ObjectManager;
        Skeletal* m_Obj = nullptr;
        std::string m_AnimName;
    };
}