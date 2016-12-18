#include "HDLCLLC.h"

namespace EPRI
{
    static constexpr uint8_t LLC_HEADER[] = { 0xE6, 0xE6, 0x00 };

    HDLCLLC::HDLCLLC(HDLCMAC * pMAC) :
        m_pMAC(pMAC)
    {
    }
    
    HDLCLLC::~HDLCLLC() 
    {
    }

    HDLCAddress HDLCLLC::MyAddress() const
    {
        return m_pMAC->MyAddress();
    }
    
    const HDLCStatistics& HDLCLLC::Statistics() const 
    {
        return m_pMAC->Statistics(); 
    }

    void HDLCLLC::ClearStatistics() 
    { 
        m_pMAC->ClearStatistics(); 
    }
    	
    Transport::ProcessResultType HDLCLLC::Process()
    {
        return 0;
    }
    
    HDLCAddress HDLCLLC::ConnectedAddress() const
    {
        return m_pMAC->ConnectedAddress();
    }

    bool HDLCLLC::IsConnected() const
    {
        return m_pMAC->IsConnected();
    }
    //
    // MA-CONNECT Service
    //
    bool HDLCLLC::MACConnectConfirmOrIndication(const BaseCallbackParameter& Parameters)
    {
        FireTransportEvent(Transport::TRANSPORT_CONNECTED);
        return true;
    }
    //
    // MA-DATA Service Implementation
    //
    bool HDLCLLC::DataRequest(const DLDataRequestParameter& Parameters)
    {
        if (IsConnected())
        {
            DLDataRequestParameter LLCParameter = Parameters;
            return m_pMAC->DataRequest(AddLLCHeader(&LLCParameter));
        }
        return false;
    }
    //
    // Transport
    //
    bool HDLCLLC::DataRequest(const Transport::DataRequestParameter& Parameters)
    {
        DLDataRequestParameter LLCParams(ConnectedAddress(), HDLCControl::UI, Parameters.Data);
        return DataRequest(LLCParams);
    }
    
    bool HDLCLLC::MACDataIndication(const BaseCallbackParameter& Parameters)
    {
        const DLDataRequestParameter& DLParam = dynamic_cast<const DLDataRequestParameter&>(Parameters);
        DLMSVector                    Data = DLParam.Data;
        //
        // Validate the Header
        //
        for (int Index = 0; Index < sizeof(LLC_HEADER); ++Index)
        {
            if (Data.Get<uint8_t>() != LLC_HEADER[Index])
            {
                return false;
            }
        }
        Data.RemoveReadBytes();
        return Transport::ProcessReception(ConnectedAddress().LogicalAddress(), 
                                           DLParam.DestinationAddress.LogicalAddress(), 
                                           &Data);
    }
    //
    // MA-DISCONNECT Service
    //
    bool HDLCLLC::MACDisconnectConfirmOrIndication(const BaseCallbackParameter& Parameters)
    {
        FireTransportEvent(Transport::TRANSPORT_DISCONNECTED);
        return true;
    }
    
    DLDataRequestParameter& HDLCLLC::AddLLCHeader(DLDataRequestParameter * pParameters)
    {
        DLMSVector LLCData;
        LLCData.AppendBuffer(LLC_HEADER, sizeof(LLC_HEADER));
        LLCData.Append(pParameters->Data);
        pParameters->Data = LLCData;
        return *pParameters;
    }

}