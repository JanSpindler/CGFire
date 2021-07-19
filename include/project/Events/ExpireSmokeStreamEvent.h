//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
using namespace particle;

namespace scene {

    class ExpireSmokeStreamEvent : public Event {
    public:
        //Just for the dummy events
        explicit ExpireSmokeStreamEvent(en::SceneRenderer &sceneRenderer,
                                  SmokeCreator &smokeCreator)
                : m_SceneRenderer(sceneRenderer),
                  m_SmokeCreator(smokeCreator) {
        }


        EventType GetTypeID() override {return EventType::ExpireSmokeStreamEvent; };

        Event* Clone() override{
            ExpireSmokeStreamEvent* e = new ExpireSmokeStreamEvent(m_SceneRenderer, m_SmokeCreator);

            strcpy_s(m_SmokeName, e->m_SmokeName);

            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: name

            strcpy_s(m_SmokeName, data[0].c_str());

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_SmokeName;
        }

        void OnAction() override {
            m_SmokeCreator.startExpiringSmokeStreamOfName(m_SmokeName);
        }


        bool OnImGuiRender() override{
            if (ImGui::InputText("Smoke Stream Name", m_SmokeName, IM_ARRAYSIZE(m_SmokeName))){
                UpdateDescription();
            }

            bool optionsOk = true;
            if (strcmp(m_SmokeName, "") == 0)
                optionsOk = false;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj*) override{
            return false;
        }

        void UpdateDescription() override{
            m_Description = "Starts expiring the smoke stream of name \'" + std::string(m_SmokeName) + "\'";
        }


    private:
        en::SceneRenderer &m_SceneRenderer;
        SmokeCreator &m_SmokeCreator;
        char m_SmokeName[32] = "";
    };


}