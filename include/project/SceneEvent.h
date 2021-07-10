//
//Created by vincent on 24.06.2021.
// In this file all the event types are defined
#pragma once

#include <utility>

#include "particle/Water.h"
#include "particle/Smoke.h"
#include "particle/Fire.h"
#include <framework/imgui_util.hpp>
#include <util/CSVWriter.h>

namespace scene{
    //Here add for each new event the type
    //Make sure to also update EventManager.h accordingly......no better solution so far:
    //Sadly you have to edit 3 different files to create a new event type
    enum class EventType{
        ShowRenderObjEvent,
        DummyEvent //Keep it the last one so we can read from this the number of event types
    };



    //Abstract base class for events
    class Event{
    public:
        virtual EventType GetTypeID() = 0;
        virtual void OnAction() = 0;
        virtual void OnImGuiRender() = 0;
        virtual bool OnImGuiRenderSetOptions() = 0; //used to create a new event. Returns true when the options are good to go
        virtual void LoadDataFromStrings(const std::vector<std::string>& data) = 0;
        virtual void SaveSpecificDataToCSV(util::CSVWriter& csv) = 0;
    private:
    };

}
