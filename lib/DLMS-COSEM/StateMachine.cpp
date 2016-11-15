#include <assert.h>
#include "StateMachine.h"
 
namespace EPRI
{
    StateMachine::StateMachine(int MaxStates) :
        m_MaxStates(MaxStates),
        m_CurrentState(0),
        m_EventGenerated(false),
        m_pEventData(nullptr)
    {
    }    
    
    void StateMachine::AddStateToMachine(uint8_t State, StateFunc Func)
    {
        m_States[State] = Func;
    }

    bool StateMachine::ExternalEvent(uint8_t NewState, 
        EventData * pData)
    {
        if (NewState == EVENT_IGNORED) 
        {
            if (pData) 
            {
                pData->Release();
            }
            return false;
        }
        else 
        {
            InternalEvent(NewState, pData); 
            StateEngine();                  
        }
        return true;
    }
 
    void StateMachine::InternalEvent(uint8_t NewState, 
        EventData * pData)
    {
        m_pEventData = pData;
        m_EventGenerated = true;
        m_CurrentState = NewState;
    }
 
    void StateMachine::StateEngine()
    {
        EventData * pDataTemp = nullptr;
 
        // TODO - lock semaphore here

        while (m_EventGenerated) 
        {         
            pDataTemp = m_pEventData;  
            m_pEventData = nullptr; 
            m_EventGenerated = false;
 
            if (m_States.find(m_CurrentState) != m_States.end())
            {
                m_States[m_CurrentState](pDataTemp);
            }
 
            if (pDataTemp) 
            {
                pDataTemp->Release();
                pDataTemp = nullptr;
            }
        }
        
        // TODO - unlock semaphore here
    }

}