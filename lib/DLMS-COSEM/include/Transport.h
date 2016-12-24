#pragma once

#include <cstdint>
#include <map>
#include <algorithm>

#include "COSEMTypes.h"
#include "Callback.h"
#include "DLMSVector.h"
#include "APDU/APDUFactory.h"
#include "COSEMAddress.h"

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
        typedef uint16_t ProcessResultType;
        
        struct DataRequestParameter : public BaseCallbackParameter
        {
            static const uint16_t ID = 0x7000;
            DataRequestParameter()
            {
            }
            DataRequestParameter(COSEMAddressType Src, COSEMAddressType Dest, const DLMSVector& D) :
                SourceAddress(Src), DestinationAddress(Dest), Data(D)
            {
            }
            COSEMAddressType    SourceAddress;
            COSEMAddressType    DestinationAddress;
            DLMSVector          Data;
        };

        typedef Callback<bool, uint16_t, TransportEvent> TransportCallback;

        virtual ~Transport()
        {
        }
        
        virtual bool IsConnected() const
        {
            return m_Connected;
        }
        
        virtual ProcessResultType Process() = 0;

        void RegisterTransportEventHandler(COSEMAddressType Address, TransportCallback::CallbackFunction Callback)
        {
            m_Callbacks[Address].m_TransportCallback.RegisterCallback(ID,
                                                                      Callback);
        }
        
        void RegisterAPDUHandler(COSEMAddressType Address, ASN::TagIDType Tag, APDUCallback::CallbackFunction Callback)
        {
            m_Callbacks[Address].m_APDUCallback.RegisterCallback(Tag,
                                                                 Callback);
        }
        //
        // DATA Service
        //
        virtual bool DataRequest(const DataRequestParameter& Parameters) = 0;
        
        bool FireTransportEvent(TransportEvent Event)
        {
            m_Connected = (TRANSPORT_CONNECTED == Event);
            
            std::for_each(m_Callbacks.begin(),
                m_Callbacks.end(), 
                [&Event](TransportCallbackType::value_type& Value)
                {
                    bool RetVal;
                    Value.second.m_TransportCallback.FireCallback(ID, Event, &RetVal);
                }
            );
            return true;
        }
        
    protected:
        virtual bool ProcessReception(COSEMAddressType SourceAddress, 
            COSEMAddressType DestinationAddress, DLMSVector * pAPDUData)
        {
            IAPDUPtr    pAPDU = APDUFactory().Parse(SourceAddress, DestinationAddress, pAPDUData);
            if (pAPDU)
            {
                std::for_each(m_Callbacks.begin(),
                    m_Callbacks.end(), 
                    [&pAPDU](TransportCallbackType::value_type& Value)
                    {
                        bool RetVal;
                        Value.second.m_APDUCallback.FireCallback(pAPDU->GetTag(),
                            pAPDU,
                            &RetVal);
                    });
                return true;
            }
            return false;
        }
        
        static const uint16_t ID = 0x1742;
        struct TransportCallbacks
        {
            TransportCallback     m_TransportCallback;
            APDUCallback          m_APDUCallback;
        };
        typedef std::map<COSEMAddressType, TransportCallbacks> TransportCallbackType;
        TransportCallbackType                                  m_Callbacks;
        bool                                                   m_Connected = false;
    };
    
}