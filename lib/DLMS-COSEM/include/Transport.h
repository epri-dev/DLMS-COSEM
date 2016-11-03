#pragma once

#include <cstdint>

#include "Callback.h"
#include "DLMSVector.h"

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
        
        struct DataRequestParameter
        {
            static const uint16_t ID = 0x7000;
            DataRequestParameter(const DLMSVector& D) :
                Data(D)
            {
            }
            DLMSVector Data;
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

        virtual bool DataRequest(const DataRequestParameter& Parameters) = 0;
        
        bool FireTransportEvent(TransportEvent Event)
        {
            bool bRetVal = false;
            return m_Callbacker.FireCallback(ID, Event, &bRetVal);
            return bRetVal;
        }
        
    protected:
        static const uint16_t ID = 0x1742;
        CallBacker            m_Callbacker;

    };
    
}