#include "HDLCLLC.h"

namespace EPRI
{
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
}