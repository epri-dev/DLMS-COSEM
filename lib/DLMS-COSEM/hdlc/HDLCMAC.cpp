#include <algorithm>

#include "ISerial.h"
#include "hdlc/HDLCMAC.h"

namespace EPRI
{
    HDLCMAC::HDLCMAC(const HDLCAddress& MyAddress, 
        ISerial * pSerial, 
        const HDLCOptions& Options,
        uint8_t MaxPreallocatedPacketBuffers) :
        m_MyAddress(MyAddress),
        m_pSerial(pSerial),
        m_CurrentOptions(Options)
    {
        while (MaxPreallocatedPacketBuffers--)
        {
            m_Packets.push(Packet());
        }
    }
    
    HDLCMAC::~HDLCMAC()
    {
    }
    
    HDLCAddress HDLCMAC::MyAddress() const
    {
        return m_MyAddress;
    }

    const HDLCStatistics& HDLCMAC::Statistics() const
    {
        return m_Statistics;
    }
    void HDLCMAC::ClearStatistics()
    {
        m_Statistics.Clear();
    }

    void HDLCMAC::LockPackets()
    {
        while (m_PacketLock.test_and_set())
            ;
    }
    
    void HDLCMAC::UnlockPackets()
    {
        m_PacketLock.clear();
    }
    
    Packet * HDLCMAC::GetWorkingRXPacket()
    {
        LockPackets();
        if (!m_pRXPacket)
        {
            if (m_Packets.size())
            {
                m_pRXPacket = &m_Packets.front();
                m_Packets.pop();
            }
        }
        UnlockPackets();
        return m_pRXPacket;
    }
    
    void HDLCMAC::ReleaseWorkingRXPacket()
    {
        LockPackets();
        if (m_pRXPacket)
        {
            m_pRXPacket->Clear();
            m_Packets.push(*m_pRXPacket);
            m_pRXPacket = nullptr;
        }
        UnlockPackets();
    }
    
    void HDLCMAC::EnqueueWorkingRXPacket()        
    {
        LockPackets();
        if (m_pRXPacket)
        {
            m_RXPackets.push(*m_pRXPacket);
            m_pRXPacket = nullptr;
        }
        UnlockPackets();
    }

    HDLCErrorCode HDLCMAC::ProcessSerialReception()
    {
        uint8_t		   Byte;
        HDLCErrorCode  ReturnValue = NEED_MORE;
        Packet *       pPacket = nullptr;

        while (m_pSerial->Read(&Byte, sizeof(Byte)) == SUCCESSFUL)
        {
            if (!pPacket)
            {
                pPacket = GetWorkingRXPacket();
                if (!pPacket)
                {
                    return NO_PACKETS;
                }
            }
            ReturnValue = pPacket->MakeByByte(Byte);
            if (ReturnValue != NEED_MORE)
            {
                break;
            }
        }
    	
        if (SUCCESS == ReturnValue)
        {
            EnqueueWorkingRXPacket();            
        }
        else
        {
            ReleaseWorkingRXPacket();
        }
        return ReturnValue;
    }

    Packet * HDLCMAC::GetWorkingTXPacket()
    {
        LockPackets();
        if (!m_pTXPacket)
        {
            if (m_Packets.size())
            {
                m_pTXPacket = &m_Packets.front();
                m_Packets.pop();
            }
        }
        UnlockPackets();
        return m_pTXPacket;
    }
    
    void HDLCMAC::ReleaseWorkingTXPacket()
    {
        LockPackets();
        if (m_pTXPacket)
        {
            m_pTXPacket->Clear();
            m_Packets.push(*m_pTXPacket);
            m_pTXPacket = nullptr;
        }
        UnlockPackets();
    }
    
    void HDLCMAC::EnqueueWorkingTXPacket()        
    {
        LockPackets();
        if (m_pTXPacket)
        {
            m_TXPackets.push(*m_pTXPacket);
            m_pTXPacket = nullptr;
        }
        UnlockPackets();
    }

    Packet * HDLCMAC::GetOutgoingPacket()        
    {
        Packet * pRetVal = nullptr;
        LockPackets();
        if (m_TXPackets.size())
        {
            pRetVal = &m_TXPackets.front();
        }
        UnlockPackets();
        return pRetVal;
    }

    void HDLCMAC::ReleaseOutgoingPacket()
    {
        LockPackets();
        if (m_TXPackets.size())
        {
            Packet * pTXPacket = &m_TXPackets.front();
            m_TXPackets.pop();
            m_Packets.push(*pTXPacket);
        }
        UnlockPackets();
    }

    HDLCErrorCode HDLCMAC::ProcessSerialTransmission()
    {
        HDLCErrorCode  ReturnValue = NEED_MORE;
        Packet *       pTXPacket = GetOutgoingPacket();
        if (pTXPacket)
        {
            if (m_pSerial->Write(*pTXPacket, pTXPacket->GetPacketLength()) != SUCCESSFUL) 
            {
                ReturnValue = TX_FAILURE;
                //
                // RETRY - STILL IN TX QUEUE
                //
            }
            ReleaseOutgoingPacket();
        }
        return ReturnValue;
    }

    Packet * HDLCMAC::GetIncomingPacket()
    {
        Packet * pRetVal = nullptr;
        LockPackets();
        if (m_RXPackets.size())
        {
            pRetVal = &m_RXPackets.front();
        }
        UnlockPackets();
        return pRetVal;
    }
    
    void HDLCMAC::ReleaseIncomingPacket()
    {
        LockPackets();
        if (m_RXPackets.size())
        {
            Packet * pRXPacket = &m_RXPackets.front();
            m_RXPackets.pop();
            m_Packets.push(*pRXPacket);
        }
        UnlockPackets();
    }

    HDLCErrorCode HDLCMAC::ProcessPacketReception()
    {
        HDLCErrorCode  ReturnValue = NEED_MORE;
        Packet *       pRXPacket = GetIncomingPacket();
        if (pRXPacket)
        {
            HDLCControl::Control PacketType = pRXPacket->GetControl().PacketType();
            if (pRXPacket->GetDestinationAddress() == m_MyAddress)
            {
                m_PacketCallback.FireCallback(PacketType, *pRXPacket, &ReturnValue);
            }
            ReleaseIncomingPacket();
        }
        return ReturnValue;
    }

} /* namespace EPRI */