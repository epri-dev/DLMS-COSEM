#pragma once

#include <cstdint>
#include <functional>
#include <map>
#include <memory>

#include "ISynchronization.h"

namespace EPRI
{
    class EventData 
    {
    public:
        virtual ~EventData() 
        {
        }
        virtual void Release()
        {
        }

    };

    typedef std::function<void(EventData *)> StateFunc;
 
    class StateMachine 
    {
    public:
        StateMachine(int maxStates);
        virtual ~StateMachine() 
        {
        }

    protected:
        enum : uint8_t
        { 
            EVENT_IGNORED = 0xFE, 
            CANNOT_HAPPEN 
        };

        bool ExternalEvent(uint8_t State, EventData * pEventData = nullptr);
        void InternalEvent(uint8_t State, EventData * pEventData = nullptr);
        virtual void AddStateToMachine(uint8_t State, StateFunc Func);

        uint8_t m_CurrentState;
        
    private:
        void StateEngine(void);

        const int                    m_MaxStates;
        bool                         m_EventGenerated;
        EventData *                  m_pEventData;
        ISemaphorePtr                m_pSemaphore;
        std::map<uint8_t, StateFunc> m_States;
    };
 
#define BEGIN_STATE_MAP \
        { 
 
#define STATE_MAP_ENTRY(State, Entry)\
            AddStateToMachine(State, std::bind(&Entry, this, std::placeholders::_1));
 
#define END_STATE_MAP \
        }
 
#define BEGIN_TRANSITION_MAP \
        static const uint8_t TRANSITIONS[] = {\
 
#define TRANSITION_MAP_ENTRY(State, Entry)\
        (uint8_t)(Entry),
 
#define END_TRANSITION_MAP(RetVal, Data) \
        0 };\
        RetVal = ExternalEvent(TRANSITIONS[m_CurrentState], Data);

    
}