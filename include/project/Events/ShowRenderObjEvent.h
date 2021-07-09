//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
#include "project/ObjectManager.h"
using namespace particle;

namespace scene {

    enum class RenderObjType {
        Sceletal = 0, Standard = 1, FixedColor = 2, Spline = 3, Reflective = 4
    };

    class ShowRenderObjEvent : public Event {
    public:
        EventType GetTypeID() override {return EventType::ShowRenderObjEvent; };

        explicit ShowRenderObjEvent(en::SceneRenderer &sceneRenderer, scene::ObjectManager& objectManager)
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
                    m_SceneRenderer.AddSceletalRenderObj(dynamic_cast<en::Sceletal *>(m_Obj.get()));
                    break;
                case RenderObjType::Standard:
                    m_SceneRenderer.AddStandardRenderObj(m_Obj.get());
                    break;
                case RenderObjType::FixedColor:
                    m_SceneRenderer.AddFixedColorRenderObj(m_Obj.get());
                    break;
                case RenderObjType::Spline:
                    m_SceneRenderer.AddSplineRenderObj(m_Obj.get());
                    break;
                case RenderObjType::Reflective:
                    m_SceneRenderer.AddReflectiveRenderObj(m_Obj.get());
                    break;
            }
        }

        void OnImGuiRender() override{
            m_Obj->OnImGuiRender();
        }

    private:
        en::SceneRenderer &m_SceneRenderer;
        scene::ObjectManager& m_ObjectManager;
        std::shared_ptr <en::RenderObj> m_Obj;
        RenderObjType m_Type;
    };
}