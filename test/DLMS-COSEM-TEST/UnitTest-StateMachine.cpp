#include <gtest/gtest.h>
#include <string>

#include "../../lib/DLMS-COSEM/StateMachine.cpp"

using namespace EPRI;

TEST(StateMachine, Basics) 
{
    class MyMachine : public StateMachine
    {
    public:
        struct MyMachineData : public EventData
        {
            MyMachineData(int Data) :
                m_Data(Data)
            {
            }
            int m_Data;
            virtual void Release()
            {
                delete this;
            }
        };
        
        MyMachine() = delete;
        MyMachine(int * pCounter)
            : StateMachine(ST_MAX_STATES),
            m_pCounter(pCounter)
        {
            ST_State1();
        }
        
        bool MyExternalEvent1()
        {
            bool bAllowed = false;
            BEGIN_TRANSITION_MAP
                TRANSITION_MAP_ENTRY(ST_STATE_1, ST_STATE_2)
                TRANSITION_MAP_ENTRY(ST_STATE_2, EVENT_IGNORED)
                TRANSITION_MAP_ENTRY(ST_STATE_3, EVENT_IGNORED)
            END_TRANSITION_MAP(bAllowed, nullptr);
            return bAllowed;
        }
        
        bool MyExternalEvent2(MyMachineData * pData)
        {
            bool bAllowed = false;
             BEGIN_TRANSITION_MAP
                TRANSITION_MAP_ENTRY(ST_STATE_1, CANNOT_HAPPEN)
                TRANSITION_MAP_ENTRY(ST_STATE_2, ST_STATE_3)
                TRANSITION_MAP_ENTRY(ST_STATE_3, EVENT_IGNORED)
            END_TRANSITION_MAP(bAllowed, pData);
            return bAllowed;
        }
 
    private:
        int * m_pCounter;
        
        void ST_State1(EventData * pData = nullptr)
        {
            (*m_pCounter)++;
        }
        void ST_State2(EventData * pData = nullptr)
        {
            (*m_pCounter)++;
        }
        void ST_State3(EventData * pData)
        {
            MyMachineData * pMyData = dynamic_cast<MyMachineData *>(pData);
            (*m_pCounter)++;
            (*m_pCounter) += pMyData->m_Data;
        }
        
        BEGIN_STATE_MAP
            STATE_MAP_ENTRY(ST_STATE_1, MyMachine::ST_State1)
            STATE_MAP_ENTRY(ST_STATE_2, MyMachine::ST_State2)
            STATE_MAP_ENTRY(ST_STATE_3, MyMachine::ST_State3)
        END_STATE_MAP
            
        enum States : uint8_t
        {
            ST_STATE_1 = 0,
            ST_STATE_2,
            ST_STATE_3,
            ST_MAX_STATES
        };
        
    };
    int       Counter = 0;
    MyMachine M(&Counter);
    
    M.MyExternalEvent1();
    M.MyExternalEvent2(new MyMachine::MyMachineData(42));
    ASSERT_EQ(45, Counter);
    
}
