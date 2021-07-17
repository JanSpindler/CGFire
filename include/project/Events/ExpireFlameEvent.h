//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
using namespace particle;

namespace scene {

    class ExpireFlameEvent : public Event {
    public:
        //Just for the dummy events
        explicit ExpireFlameEvent(en::SceneRenderer &sceneRenderer,
                                  FireCreator &fireCreator)
                : m_SceneRenderer(sceneRenderer),
                  m_FireCreator(fireCreator) {
        }


        EventType GetTypeID() override {return EventType::ExpireFlameEvent; };

        Event* Clone() override{
            ExpireFlameEvent* e = new ExpireFlameEvent(m_SceneRenderer, m_FireCreator);

            strcpy_s(m_FlameName, e->m_FlameName);

            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: name

            strcpy_s(m_FlameName, data[0].c_str());

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_FlameName;
        }

        void OnAction() override {
            m_FireCreator.startExpiringFlameOfName(m_FlameName);
        }


        bool OnImGuiRender() override{
            if (ImGui::InputText("Flame Name", m_FlameName, IM_ARRAYSIZE(m_FlameName))){
                UpdateDescription();
            }

            bool optionsOk = true;
            if (strcmp(m_FlameName, "") == 0)
                optionsOk = false;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj*) override{
            return false;
        }

        void UpdateDescription() override{
            m_Description = "Starts expiring the flame of name \'" + std::string(m_FlameName)+ "\'";
        }


    private:
        en::SceneRenderer &m_SceneRenderer;
        FireCreator &m_FireCreator;
        char m_FlameName[32] = "";
    };


}