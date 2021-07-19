//
//Created by vincent on 24.06.2021.
// In this file all the event types are defined
#pragma once


#include <util/CSVWriter.h>
#include "InitEvents.h"

namespace en {
    class RenderObj;
}

namespace scene{

    //Abstract base class for events
    class Event{
    public:
        virtual ~Event() = default;
        virtual EventType GetTypeID() = 0;

        //Returns a copy. This is used to create new events from the dummy events
        virtual Event* Clone() = 0;

        virtual void OnResetTime(){ }
        virtual void OnAction() = 0; //the thing that happens
        virtual bool OnImGuiRender() = 0; //used to create a new event. Returns true when the options are good to go
        virtual void LoadDataFromStrings(const std::vector<std::string>& data) = 0;
        virtual void SaveSpecificDataToCSV(util::CSVWriter& csv) = 0;

        //necessary for if while editing we delete an object, the related Events must be deleted by EventManager
        virtual bool IsRenderObjRelated(en::RenderObj* obj) = 0;

        virtual void UpdateDescription() = 0;

        const std::string& GetDescription(){
            return m_Description;
        }
    protected:
        std::string m_Description;
    };

}
