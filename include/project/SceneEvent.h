//
//Created by vincent on 24.06.2021.
// In this file all the event types are defined
#pragma once

#include <utility>

#include "particle/Water.h"
#include "particle/Smoke.h"
#include "particle/Fire.h"
#include <framework/imgui_util.hpp>

namespace scene{
    //Here add for each new event the type
    enum class EventType{
        ShowRenderObjEvent = 0
    };



    //Abstract base class for events
    class Event{
    public:
        virtual void OnAction() = 0;
        virtual void OnImGuiRender() = 0;
        virtual void LoadFromStrings(const std::vector<std::string>& data) = 0;
    private:
    };

}
