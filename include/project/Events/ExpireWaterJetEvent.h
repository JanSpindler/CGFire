//
//Created by vincent on 08.07.2021.
//
#pragma once
#include "project/SceneEvent.h"
using namespace particle;

namespace scene {

    class ExpireWaterJetEvent : public Event {
    public:
        //Just for the dummy events
        explicit ExpireWaterJetEvent(en::SceneRenderer &sceneRenderer,
                                        WaterCreator &waterCreator)
                : m_SceneRenderer(sceneRenderer),
                  m_WaterCreator(waterCreator) {
        }


        EventType GetTypeID() override {return EventType::ExpireWaterJetEvent; };

        Event* Clone() override{
            ExpireWaterJetEvent* e = new ExpireWaterJetEvent(m_SceneRenderer, m_WaterCreator);

            strcpy_s(e->m_WaterJetName, m_WaterJetName);

            return e;
        }

        void LoadDataFromStrings(const std::vector<std::string>& data) override{
            //<specific data>: name

            strcpy_s(m_WaterJetName, data[0].c_str());

            this->UpdateDescription();
        }

        void SaveSpecificDataToCSV(util::CSVWriter& csv) override{
            csv << m_WaterJetName;
        }

        void OnAction() override {
            m_WaterCreator.startExpiringWaterJetOfName(m_WaterJetName);
        }


        bool OnImGuiRender() override{
            if (ImGui::InputText("Water Jet Name", m_WaterJetName, IM_ARRAYSIZE(m_WaterJetName))){
                UpdateDescription();
            }

            bool optionsOk = true;
            if (strcmp(m_WaterJetName, "") == 0)
                optionsOk = false;
            return optionsOk;
        }
        bool IsRenderObjRelated(en::RenderObj*) override{
            return false;
        }

        void UpdateDescription() override{
            m_Description = "Starts expiring the Water Jet of name \'" + std::string(m_WaterJetName) + "\'";
        }


    private:
        en::SceneRenderer &m_SceneRenderer;
        WaterCreator &m_WaterCreator;
        char m_WaterJetName[32] = "";
    };


}