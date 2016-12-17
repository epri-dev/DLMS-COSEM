#include "hdlc/packet.h"
#include "ISerial.h"
#include "hdlc/HDLCHelpers.h"

#define TRACE printf

namespace EPRI
{
    const uint8_t Packet::IDENTIFY_RESPONSE[] = { 0x00, 0x04, 0x01, 0x00 };

    static const uint16_t CRC16_TABLE[] =
    {
        0x0000,
        0x1189,
        0x2312,
        0x329b,
        0x4624,
        0x57ad,
        0x6536,
        0x74bf,
        0x8c48,
        0x9dc1,
        0xaf5a,
        0xbed3,
        0xca6c,
        0xdbe5,
        0xe97e,
        0xf8f7,
        0x1081,
        0x0108,
        0x3393,
        0x221a,
        0x56a5,
        0x472c,
        0x75b7,
        0x643e,
        0x9cc9,
        0x8d40,
        0xbfdb,
        0xae52,
        0xdaed,
        0xcb64,
        0xf9ff,
        0xe876,
        0x2102,
        0x308b,
        0x0210,
        0x1399,
        0x6726,
        0x76af,
        0x4434,
        0x55bd,
        0xad4a,
        0xbcc3,
        0x8e58,
        0x9fd1,
        0xeb6e,
        0xfae7,
        0xc87c,
        0xd9f5,
        0x3183,
        0x200a,
        0x1291,
        0x0318,
        0x77a7,
        0x662e,
        0x54b5,
        0x453c,
        0xbdcb,
        0xac42,
        0x9ed9,
        0x8f50,
        0xfbef,
        0xea66,
        0xd8fd,
        0xc974,
        0x4204,
        0x538d,
        0x6116,
        0x709f,
        0x0420,
        0x15a9,
        0x2732,
        0x36bb,
        0xce4c,
        0xdfc5,
        0xed5e,
        0xfcd7,
        0x8868,
        0x99e1,
        0xab7a,
        0xbaf3,
        0x5285,
        0x430c,
        0x7197,
        0x601e,
        0x14a1,
        0x0528,
        0x37b3,
        0x263a,
        0xdecd,
        0xcf44,
        0xfddf,
        0xec56,
        0x98e9,
        0x8960,
        0xbbfb,
        0xaa72,
        0x6306,
        0x728f,
        0x4014,
        0x519d,
        0x2522,
        0x34ab,
        0x0630,
        0x17b9,
        0xef4e,
        0xfec7,
        0xcc5c,
        0xddd5,
        0xa96a,
        0xb8e3,
        0x8a78,
        0x9bf1,
        0x7387,
        0x620e,
        0x5095,
        0x411c,
        0x35a3,
        0x242a,
        0x16b1,
        0x0738,
        0xffcf,
        0xee46,
        0xdcdd,
        0xcd54,
        0xb9eb,
        0xa862,
        0x9af9,
        0x8b70,
        0x8408,
        0x9581,
        0xa71a,
        0xb693,
        0xc22c,
        0xd3a5,
        0xe13e,
        0xf0b7,
        0x0840,
        0x19c9,
        0x2b52,
        0x3adb,
        0x4e64,
        0x5fed,
        0x6d76,
        0x7cff,
        0x9489,
        0x8500,
        0xb79b,
        0xa612,
        0xd2ad,
        0xc324,
        0xf1bf,
        0xe036,
        0x18c1,
        0x0948,
        0x3bd3,
        0x2a5a,
        0x5ee5,
        0x4f6c,
        0x7df7,
        0x6c7e,
        0xa50a,
        0xb483,
        0x8618,
        0x9791,
        0xe32e,
        0xf2a7,
        0xc03c,
        0xd1b5,
        0x2942,
        0x38cb,
        0x0a50,
        0x1bd9,
        0x6f66,
        0x7eef,
        0x4c74,
        0x5dfd,
        0xb58b,
        0xa402,
        0x9699,
        0x8710,
        0xf3af,
        0xe226,
        0xd0bd,
        0xc134,
        0x39c3,
        0x284a,
        0x1ad1,
        0x0b58,
        0x7fe7,
        0x6e6e,
        0x5cf5,
        0x4d7c,
        0xc60c,
        0xd785,
        0xe51e,
        0xf497,
        0x8028,
        0x91a1,
        0xa33a,
        0xb2b3,
        0x4a44,
        0x5bcd,
        0x6956,
        0x78df,
        0x0c60,
        0x1de9,
        0x2f72,
        0x3efb,
        0xd68d,
        0xc704,
        0xf59f,
        0xe416,
        0x90a9,
        0x8120,
        0xb3bb,
        0xa232,
        0x5ac5,
        0x4b4c,
        0x79d7,
        0x685e,
        0x1ce1,
        0x0d68,
        0x3ff3,
        0x2e7a,
        0xe70e,
        0xf687,
        0xc41c,
        0xd595,
        0xa12a,
        0xb0a3,
        0x8238,
        0x93b1,
        0x6b46,
        0x7acf,
        0x4854,
        0x59dd,
        0x2d62,
        0x3ceb,
        0x0e70,
        0x1ff9,
        0xf78f,
        0xe606,
        0xd49d,
        0xc514,
        0xb1ab,
        0xa022,
        0x92b9,
        0x8330,
        0x7bc7,
        0x6a4e,
        0x58d5,
        0x495c,
        0x3de3,
        0x2c6a,
        0x1ef1,
        0x0f78
    };

    Packet::Packet()
    {
        Clear();
    }

    uint16_t Packet::GetU16(int Index) const
    {
        return Get16BigEndianHelper(&m_Information[Index]);
    }
    
    uint16_t Packet::GetRemainingPacketLength() const
    {
        if (-1 == m_PacketMappings.m_Format)
            return 0;
        return (GetU16(m_PacketMappings.m_Format) & 0b0000011111111111);
    }
    
    uint16_t Packet::GetPacketLength() const
    {
        if (-1 == m_PacketMappings.m_Format)
        {
            if (GetControl().PacketType() == HDLCControl::IDENTR)
            {
                return sizeof(IDENTIFY_RESPONSE);
            }
            else
            {
                return sizeof(uint8_t);
            }
        }
        return GetRemainingPacketLength() + FLAG_FIELDS;
    }

    Packet::Segmentation Packet::GetSegmentation() const
    {
        if (-1 == m_PacketMappings.m_Format)
            return NO_SEGMENT;
        return Segmentation((GetU16(m_PacketMappings.m_Format) & 0b0000100000000000) >> 11);
    }
    
    HDLCAddress Packet::GetDestinationAddress() const
    {
        if (-1 == m_PacketMappings.m_DestinationAddress)
            return HDLCAddress();
        return HDLCAddress().Parse(&m_Information[m_PacketMappings.m_DestinationAddress]);
    }
    
    HDLCAddress Packet::GetSourceAddress() const
    {
        if (-1 == m_PacketMappings.m_SourceAddress)
            return HDLCAddress();
        return HDLCAddress().Parse(&m_Information[m_PacketMappings.m_SourceAddress]);
    }
    
    HDLCControl Packet::GetControl() const
    {
        return HDLCControl(m_Information[m_PacketMappings.m_Control]);
    }
    
    uint16_t Packet::GetInformationLength() const
    {
        if (GetControl().PacketType() == HDLCControl::IDENTR)
        {
            return sizeof(IDENTIFY_RESPONSE);
        }
        uint16_t PacketLength = GetPacketLength();
        if ((m_HeaderLength + CRC_FIELD + FLAG_FIELDS) > PacketLength)
        {
            return 0;
        }
        return GetPacketLength() - (m_HeaderLength + CRC_FIELD + FLAG_FIELDS);
    }

    const uint8_t * Packet::GetInformation(size_t& InformationLength) const
    {
        if (-1 == m_PacketMappings.m_Information)
            return nullptr;
        InformationLength = GetInformationLength();
        return &m_Information[m_PacketMappings.m_Information];
    }
    
    bool Packet::IsIdentify() const
    {
        switch (GetControl().PacketType())
        {
        case HDLCControl::IDENT:
        case HDLCControl::IDENTR:
            return true;
        default:
            break;
        }
        return false;
    }

    uint16_t Packet::ComputeCRC(const uint8_t * Buffer, size_t Size, uint16_t CurrentCRC /* = PPPINITFCS16 */)
    {
        while (Size--)
        {
            CurrentCRC = (CurrentCRC >> 8) ^ CRC16_TABLE[(CurrentCRC ^ *Buffer++) & 0xFF];
        }
        return CurrentCRC ^ 0xFFFF;
    }
	
    uint16_t Packet::ComputeCRC(uint8_t Byte, uint16_t CurrentCRC /*= PPPINITFCS16*/)
    {
        return (uint16_t)((CurrentCRC >> 8) ^ CRC16_TABLE[(CurrentCRC ^ Byte) & 0xff]);
    }

    void Packet::Insert(uint8_t Byte, int& Index)
    {
        m_Information[Index++] = Byte;
    }
	
    void Packet::Insert(uint16_t Value, int& Index, bool bBigEndian /* = true */)
    {
        if (bBigEndian)
        {
            m_Information[Index++] = (0xFF & (Value >> 8));
            m_Information[Index++] = (0xFF & Value);
        }
        else
        {
            m_Information[Index++] = (0xFF & Value);
            m_Information[Index++] = (0xFF & (Value >> 8));
        }
    }
    
    void Packet::Insert(const HDLCAddress& Value, int& Index)
    {
        std::memcpy(&m_Information[Index], Value.Get(), Value.Size());
        Index += Value.Size();
    }

    void Packet::InsertFrameFormat(size_t HeaderSize, const HDLCControl& Control, size_t InformationSize, Segmentation Segment, int& Index)
    {
        uint16_t FrameFormat = 0;
        switch (Control.PacketType())
        {
        case HDLCControl::DISC:
        case HDLCControl::DM:   
        case HDLCControl::RR:   
        case HDLCControl::RNR:  
            FrameFormat = HeaderSize;
            break;
        case HDLCControl::SNRM: 
        case HDLCControl::FRMR: 
        case HDLCControl::UA:   
        case HDLCControl::UI:   
        case HDLCControl::INFO:   
            if (0 == InformationSize)
            {
                FrameFormat = HeaderSize;
            }
            else
            {
                FrameFormat = HeaderSize + InformationSize + CRC_FIELD;
            }
            break;
        default:
            break;
        }
        Insert(uint16_t((0b1010 << 12) | (Segment << 11) | FrameFormat), Index);
    }

			
    HDLCErrorCode Packet::MakePacket(Segmentation Segmented,
        const HDLCAddress& DestinationAddress,
        const HDLCAddress& SourceAddress,
        const HDLCControl& Control,
        const uint8_t * Information /* = nullptr */,
        size_t InformationSize /* = 0 */)
    {
        int		 PacketIndex = 0;
        size_t   HeaderSize = STATIC_HEADER_SIZE + SourceAddress.Size() + DestinationAddress.Size();

        Insert(HDLC_START_FLAG, PacketIndex);
    	
        m_PacketMappings.m_Format = PacketIndex;
        InsertFrameFormat(HeaderSize,
            Control,
            InformationSize,
            Segmented,
            PacketIndex);
    	
        m_PacketMappings.m_DestinationAddress = PacketIndex;
        Insert(DestinationAddress, PacketIndex);
    	
        m_PacketMappings.m_SourceAddress = PacketIndex;
        Insert(SourceAddress, PacketIndex);
    	
        m_PacketMappings.m_Control = PacketIndex;
        Insert(Control, PacketIndex);
    	
        Insert(ComputeCRC(&m_Information[1], HeaderSize - HCS_FIELD), PacketIndex, false);
        m_HeaderLength = (PacketIndex - 1);
    	
        m_PacketMappings.m_Information = PacketIndex;
        for (int nInformationIndex = 0; (Information != nullptr) && (nInformationIndex < InformationSize); ++nInformationIndex)
        {
            Insert(Information[nInformationIndex], PacketIndex);
        }
        if (InformationSize)
        {
            Insert(ComputeCRC(&m_Information[1], PacketIndex - 1), PacketIndex, false);
        }
        Insert(HDLC_STOP_FLAG, PacketIndex);

        return SUCCESS;
    }

    HDLCErrorCode Packet::MakeIdentifyPacket(const HDLCControl& Control,
        uint8_t SuccessCode,
        uint8_t ProtocolID,
        uint8_t ProtocolVersion,
        uint8_t ProtocolRevision)
    {
        int		 PacketIndex = 0;
        
        if (Control.PacketType() == HDLCControl::IDENT)
        {
            m_PacketMappings.m_Format = -1;
            m_PacketMappings.m_DestinationAddress = -1;
            m_PacketMappings.m_SourceAddress = -1;
            m_PacketMappings.m_Information = -1;
            m_PacketMappings.m_Control = PacketIndex;
            Insert(Packet::IDENTIFY_FLAG1, PacketIndex);
        }
        else if (Control.PacketType() == HDLCControl::IDENTR)
        {
            m_PacketMappings.m_Format = -1;
            m_PacketMappings.m_DestinationAddress = -1;
            m_PacketMappings.m_SourceAddress = -1;
            m_PacketMappings.m_Control = PacketIndex;
            Insert((uint8_t) HDLCControl::IDENTR, PacketIndex);
            m_PacketMappings.m_Information = PacketIndex;
            Insert(SuccessCode, PacketIndex);
            Insert(ProtocolID, PacketIndex);
            Insert(ProtocolVersion, PacketIndex);
            Insert(ProtocolRevision, PacketIndex);
        }
        else
        {
            return NOT_POSSIBLE;
        }
        return SUCCESS;
    }
    
    HDLCErrorCode Packet::MakeByByte(uint8_t Byte)
    {
        HDLCErrorCode ReturnValue = NEED_MORE;
        
        switch (m_PacketState)
        {
        case STATE_RX_NO_PACKET:
            if (Packet::HDLC_START_FLAG == Byte)
            {
                m_PacketState = STATE_RX_FRAME_FORMAT;
                m_PacketIndex = 0;
                m_PacketMappings = { };
                m_PacketMappings.m_Format = m_PacketIndex;
            }
            else if (Packet::IDENTIFY_FLAG1 == Byte ||
                Packet::IDENTIFY_FLAG2 == Byte)
            {
                m_PacketIndex = 0;
                m_PacketMappings.m_Format = -1;
                m_PacketMappings.m_DestinationAddress = -1;
                m_PacketMappings.m_SourceAddress = -1;
                m_PacketMappings.m_Information = -1;
                m_PacketMappings.m_Control = m_PacketIndex;
                m_Information[m_PacketIndex] = HDLCControl::IDENT;
                ReturnValue = SUCCESS;
            }
            else if (Packet::IDENTIFY_RESPONSE[0] == Byte)
            {
                m_PacketState = STATE_RX_IDENTIFY;
                m_PacketIndex = 0;
                m_PacketMappings.m_Format = -1;
                m_PacketMappings.m_DestinationAddress = -1;
                m_PacketMappings.m_SourceAddress = -1;
                m_PacketMappings.m_Control = m_PacketIndex;
                m_Information[m_PacketIndex++] = HDLCControl::IDENTR;
                m_PacketMappings.m_Information = m_PacketIndex;
                m_Information[m_PacketIndex++] = Byte;
                m_CurrentFieldBytes++;
            }
            break;

        case STATE_RX_IDENTIFY:
            m_Information[m_PacketIndex++] = Byte;
            if (sizeof(IDENTIFY_RESPONSE) == ++m_CurrentFieldBytes)
            {
                if (0 == std::memcmp(IDENTIFY_RESPONSE, 
                    &m_Information[m_PacketMappings.m_Information],
                    sizeof(IDENTIFY_RESPONSE)))
                {
                    ReturnValue = SUCCESS;
                }
                else
                {
                    ReturnValue = FAIL;
                }
            }
            break;
            
        case STATE_RX_FRAME_FORMAT:
            m_Information[m_PacketIndex++] = Byte;
            if (FRAME_FORMAT == ++m_CurrentFieldBytes)
            {
                m_CurrentFieldBytes = 0;
                m_PacketMappings.m_DestinationAddress = m_PacketIndex;
                m_PacketState = STATE_RX_DESTINATION_ADDRESS;
            }
            break;
		
        case STATE_RX_DESTINATION_ADDRESS :
            m_Information[m_PacketIndex++] = Byte;
            if (Byte & 0x01)
            {
                m_PacketMappings.m_SourceAddress = m_PacketIndex;
                m_PacketState = STATE_RX_SOURCE_ADDRESS;
            }
            break;
    			
        case STATE_RX_SOURCE_ADDRESS :
            m_Information[m_PacketIndex++] = Byte;
            if (Byte & 0x01)
            {
                m_PacketMappings.m_Control = m_PacketIndex;
                m_PacketState = STATE_RX_CONTROL;
            }
            break;
    			
        case STATE_RX_CONTROL:
            m_Information[m_PacketIndex++] = Byte;
            m_CurrentFieldBytes = 0;
            m_PacketState = STATE_RX_HCS;
            break;
    			
        case STATE_RX_HCS:
            m_Information[m_PacketIndex++] = Byte;
            if (HCS_FIELD == ++m_CurrentFieldBytes)
            {
                if (PPPGOODFCS16 == ComputeCRC(&m_Information[0], m_PacketIndex))
                {
                    m_CurrentFieldBytes = 0;
                    m_HeaderLength = m_PacketIndex;
                    m_PacketMappings.m_Information = m_PacketIndex;
                    
                    if (GetInformationLength())
                    {
                        m_PacketState = STATE_RX_INFORMATION;
                    }
                    else
                    {
                        ReturnValue = SUCCESS;
                    }
                }
                else
                {
                    ReturnValue = HEADER_CRC_FAILURE;
                }
            }
            break;
    			
        case STATE_RX_INFORMATION:
            m_Information[m_PacketIndex++] = Byte;
            if (GetInformationLength() == ++m_CurrentFieldBytes)
            {
                m_CurrentFieldBytes = 0;
                m_PacketState = STATE_RX_CRC;
            }
            break;
    			
        case STATE_RX_CRC:
            m_Information[m_PacketIndex++] = Byte;
            if (CRC_FIELD == ++m_CurrentFieldBytes)
            {
                if (PPPGOODFCS16 == ComputeCRC(&m_Information[0], m_PacketIndex))
                {
                    ReturnValue = SUCCESS;
                }
                else
                {
                    ReturnValue = PACKET_CRC_FAILURE;
                }
            }
            break;

        default:
            ReturnValue = FAIL;
            break;

        } /* SWITCH */

        return ReturnValue;

    }
    
    HDLCErrorCode Packet::MakeByVector(DLMSVector * pVector)
    {
        HDLCErrorCode ReturnValue = NEED_MORE;
        while (pVector->GetReadPosition() < pVector->Size())
        {
            ReturnValue = MakeByByte(pVector->Get<uint8_t>());
            if (SUCCESS == ReturnValue)
            {
                break;
            }
        }
        return ReturnValue;
    }
    
    void Packet::Clear()
    {
        m_PacketState = STATE_RX_NO_PACKET;
        m_PacketIndex = 0;
        m_CurrentFieldBytes = 0;
        m_HeaderLength = 0;
        std::memset(m_Information, '\0', sizeof(m_Information));
    }

    Packet::operator const uint8_t *() const
    {
        if (GetControl().PacketType() == HDLCControl::IDENTR)
        {
            return &m_Information[m_PacketMappings.m_Information];
        }
        return m_Information;
    }
    
    Packet::operator DLMSVector() const
    {
        if (GetControl().PacketType() == HDLCControl::IDENTR)
        {
            return DLMSVector(&m_Information[m_PacketMappings.m_Information], GetPacketLength());
        }
        return DLMSVector(m_Information, GetPacketLength());
    }
    
} /* namespace EPRI */