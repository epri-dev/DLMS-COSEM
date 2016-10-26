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
    	
    HDLCRunResult HDLCLLC::Process()
    {
        return m_pMAC->Process();
    }
    
    DLDataRequestParameter& HDLCLLC::AddLLCHeader(DLDataRequestParameter * pParameters)
    {
        pParameters->Data.insert(pParameters->Data.begin(), 
            LLC_HEADER, LLC_HEADER + sizeof(LLC_HEADER));
        return *pParameters;
    }

}