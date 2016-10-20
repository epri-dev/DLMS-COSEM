#pragma once

#include <cstdint>

#include "Callback.h"

namespace EPRI
{
    class Transport
    {
    public:
        enum TransportEvent : uint8_t
        {
            TRANSPORT_CONNECTED    = 1,
            TRANSPORT_DISCONNECTED
        };
        
        typedef Callback<bool, uint16_t, TransportEvent> CallBacker;

        virtual ~Transport()
        {
        }
        
        void RegisterTransportEventHandler(CallBacker::CallbackFunction Callback)
        {
            m_Callbacker.RegisterCallback(ID,
                Callback);
        }
        
        bool FireTransportEvent(TransportEvent Event)
        {
            bool bRetVal = false;
            m_Callbacker.FireCallback(ID, Event, &bRetVal);
            return bRetVal;
        }
        
    protected:
        static const uint16_t ID = 0x1742;
        CallBacker            m_Callbacker;

    };
    
}