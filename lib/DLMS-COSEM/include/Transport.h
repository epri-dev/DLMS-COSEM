#pragma once

#include <cstdint>

#include "Callback.h"
#include "DLMSVector.h"
#include "APDU/APDUFactory.h"

namespace EPRI
{
    class Transport : public Callback<bool, uint16_t>
    {
        using APDUCallback = Callback<bool, ASN::TagIDType, IAPDUPtr>;

    public:
        enum TransportEvent : uint8_t
        {
            TRANSPORT_CONNECTED    = 1,
            TRANSPORT_DISCONNECTED
        };
        
        struct DataRequestParameter : public BaseCallbackParameter
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
        
        void RegisterAPDUHandler(ASN::TagIDType Tag, APDUCallback::CallbackFunction Callback)
        {
            m_APDUCallback.RegisterCallback(Tag, 
                Callback);
        }
        //
        // DATA Service
        //
        virtual bool DataRequest(const DataRequestParameter& Parameters) = 0;
        
        bool FireTransportEvent(TransportEvent Event)
        {
            bool bRetVal = false;
            return m_Callbacker.FireCallback(ID, Event, &bRetVal);
            return bRetVal;
        }
        
    protected:
        virtual bool ProcessReception(DLMSVector * pData)
        {
            IAPDUPtr    pAPDU = APDUFactory().Parse(pData);
            if (pAPDU)
            {
                bool CallbackRetVal = false;
                return (m_APDUCallback.FireCallback(pAPDU->GetTag(), pAPDU, &CallbackRetVal) && !CallbackRetVal);
            }
            return false;
        }
        
        static const uint16_t ID = 0x1742;
        CallBacker            m_Callbacker;
        APDUCallback          m_APDUCallback;
    };
    
}