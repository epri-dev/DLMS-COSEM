#pragma once

#include <cstdint>
#include <functional>

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

    struct StateStruct;
 
    class StateMachine 
    {
    public:
        StateMachine(int maxStates);
        virtual ~StateMachine() 
        {
        }

    protected:
        enum 
        { 
            EVENT_IGNORED = 0xFE, 
            CANNOT_HAPPEN 
        };

        bool ExternalEvent(uint8_t State, EventData * pEventData = nullptr);
        void InternalEvent(uint8_t State, EventData * pEventData = nullptr);
        virtual const StateStruct * GetStateMap() = 0;

        uint8_t m_CurrentState;
        
    private:
        void StateEngine(void);

        const int   m_MaxStates;
        bool        m_EventGenerated;
        EventData * m_pEventData;

    };
 
    typedef std::function<void(EventData *)> StateFunc;
    struct StateStruct 
    {
        StateFunc m_StateFunc;    
    };
 
     #define BEGIN_STATE_MAP \
     public:\
     const StateStruct * GetStateMap() {\
         static const StateStruct m_StateMap[] = { 
 
    #define STATE_MAP_ENTRY(State, Entry)\
        { std::bind(&Entry, this, std::placeholders::_1) },
 
    #define END_STATE_MAP \
        { nullptr }\
        }; \
        return &m_StateMap[0]; }
 
    #define BEGIN_TRANSITION_MAP \
        static const uint8_t TRANSITIONS[] = {\
 
    #define TRANSITION_MAP_ENTRY(State, Entry)\
        Entry,
 
    #define END_TRANSITION_MAP(RetVal, Data) \
        0 };\
        RetVal = ExternalEvent(TRANSITIONS[m_CurrentState], Data);
    
}