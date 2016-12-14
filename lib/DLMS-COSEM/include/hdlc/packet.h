#pragma once

#include <cstdint>

#include "HDLCErrors.h"
#include "HDLCStatistics.h"
#include "linkdefs.h"
#include "HDLCControl.h"
#include "HDLCAddress.h"
#include "DLMSVector.h"

namespace EPRI
{
	class ISerial;
	
	class Packet
	{
	public:
		static const uint8_t HDLC_START_FLAG = 0x7E;
		static const uint8_t HDLC_STOP_FLAG = 0x7E;
		static const uint8_t FRAME_FORMAT = sizeof(uint16_t);
    	static const uint8_t MAX_ADDRESS_FIELD = sizeof(uint16_t);
		static const uint8_t CONTROL_FIELD = sizeof(uint8_t);
		static const uint8_t HCS_FIELD = sizeof(uint16_t);
		static const uint8_t CRC_FIELD = sizeof(uint16_t);
		static const uint8_t FLAG_FIELDS = sizeof(HDLC_START_FLAG) + sizeof(HDLC_STOP_FLAG);
		static const size_t MAX_PACKET_SIZE = 60;
		static const size_t STATIC_HEADER_SIZE = (FRAME_FORMAT + CONTROL_FIELD + HCS_FIELD);
    	static const size_t MAX_HDLC_FRAME_SIZE = STATIC_HEADER_SIZE + (2 * MAX_ADDRESS_FIELD) + MAX_PACKET_SIZE + FLAG_FIELDS + CRC_FIELD;
    	static const size_t HEADER_START_SIZE = (sizeof(uint8_t) + FRAME_FORMAT);

		enum Segmentation : uint8_t
		{
			NO_SEGMENT = 0,
			SEGMENT    = 1
		};
		
    	Packet();
		virtual ~Packet()
		{
		}
    	
    	void Clear();
   	
    	virtual uint16_t GetPacketLength() const;
    	virtual uint16_t GetRemainingPacketLength() const;
    	virtual Segmentation GetSegmentation() const;
    	virtual HDLCAddress GetDestinationAddress() const;
    	virtual HDLCAddress GetSourceAddress() const;
    	virtual HDLCControl GetControl() const;
    	virtual uint16_t GetInformationLength() const;
    	virtual const uint8_t * GetInformation(size_t& InformationLength) const;
    	
    	virtual HDLCErrorCode MakePacket(Segmentation Segmented,
        	const HDLCAddress& DestinationAddr,
        	const HDLCAddress& SourceAddress,
        	const HDLCControl& Control,
        	const uint8_t * Information = nullptr,
        	size_t InformationSize = 0);
    	virtual HDLCErrorCode MakeByByte(uint8_t Byte);
    	virtual HDLCErrorCode MakeByVector(DLMSVector * pVector);
    	
    	operator const uint8_t *() const;
    	//
    	// TODO - Rework to Base Packet on DLMSVector
    	//
    	operator DLMSVector() const;

	protected:
    	enum PACKET_RX_STATE
    	{
        	STATE_RX_NO_PACKET,
        	STATE_RX_FRAME_FORMAT,
        	STATE_RX_SOURCE_ADDRESS,
        	STATE_RX_DESTINATION_ADDRESS,
        	STATE_RX_CONTROL,
        	STATE_RX_HCS,
        	STATE_RX_INFORMATION,
        	STATE_RX_CRC
    	};

    	struct PacketMappingIndexes
    	{
        	int m_Format;
        	int m_DestinationAddress;
        	int m_SourceAddress;
        	int m_Control;
        	int m_Information;
    	}               m_PacketMappings = {};

    	uint16_t GetU16(int Index) const;
        uint16_t ComputeCRC(const uint8_t * Buffer, size_t Size, uint16_t CurrentCRC = PPPINITFCS16);
		uint16_t ComputeCRC(uint8_t Byte, uint16_t CurrentCRC = PPPINITFCS16);
    	void Insert(uint8_t Word, int& Index);
    	void Insert(uint16_t Value, int& Index, bool bBigEndian = true);
    	void Insert(const HDLCAddress& Value, int& Index);
    	void InsertFrameFormat(size_t HeaderSize, 
        	const HDLCControl& Control, 
        	size_t InformationSize, 
        	Segmentation Segment, 
        	int& Index);

    	static const uint16_t PPPINITFCS16 = 0xFFFF;
    	static const uint16_t PPPGOODFCS16 = (0xF0B8 ^ 0xFFFF);
        uint8_t			      m_Information[MAX_HDLC_FRAME_SIZE] = { }; 
    	int                   m_HeaderLength = 0;
    	PACKET_RX_STATE	      m_PacketState;
    	int                   m_PacketIndex;
    	int                   m_CurrentFieldBytes;
	};

}