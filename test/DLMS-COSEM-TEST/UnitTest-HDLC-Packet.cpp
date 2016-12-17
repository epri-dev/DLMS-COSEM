#include <gtest/gtest.h>
#include <cstdint>
#include <vector>

#include "../../lib/DLMS-COSEM/hdlc/HDLCHelpers.cpp"
#include "../../lib/DLMS-COSEM/hdlc/packet.cpp"
#include "DummySerial.h"

using namespace EPRI;

static const uint8_t PUT_TEST[] = { 0xE6, 0xE6, 0x00, 0x1D, 0x64, 0x00, 0x14, 0x00, 0x00 };
static const uint8_t PUT_TEST_PACKET[] = { 0x7e, 0xa0, 0x13, 0xce, 0xff, 0xcd, 0x13, 0x61, 0xd5, 0xe6, 0xe6, 0x00, 0x1d, 0x64, 0x00, 0x14, 0x00, 0x00, 0x2c, 0x66, 0x7e };
static const uint8_t PUT_SNRM[] = { 0x7E, 0xA0, 0x0A, 0x00, 0x02, 0x00, 0x23, 0x21, 0x93, 0x18, 0x71, 0x7E };

TEST(HDLCPacket, MakePacket) 
{
    Packet   Sender;

    // EXAMPLE FROM GREEN BOOK
    HDLCErrorCode Error = Sender.MakePacket(Packet::NO_SEGMENT,
        HDLCAddress(uint8_t(0x67), uint8_t(0x7f)),
        HDLCAddress(0x66),
        HDLCControl(HDLCControl::UI),
        PUT_TEST,
        sizeof(PUT_TEST));
    EXPECT_EQ(SUCCESS, Error);
    EXPECT_EQ(0,
        std::memcmp(PUT_TEST_PACKET, (const uint8_t *) Sender, sizeof(PUT_TEST_PACKET)));
    EXPECT_EQ(sizeof(PUT_TEST_PACKET), Sender.GetPacketLength());
   
    // Now let's read the exact same thing to see if we can parse it!
    size_t InformationLength = 0;
    EXPECT_EQ(Packet::NO_SEGMENT, Sender.GetSegmentation());
    EXPECT_TRUE(HDLCAddress(uint8_t(0x67), uint8_t(0x7f)) == Sender.GetDestinationAddress());
    EXPECT_TRUE(HDLCAddress(0x66) == Sender.GetSourceAddress());
    EXPECT_EQ(HDLCControl(HDLCControl::UI), Sender.GetControl());
    EXPECT_EQ(sizeof(PUT_TEST), Sender.GetInformationLength());   
    EXPECT_EQ(0, std::memcmp(Sender.GetInformation(InformationLength), PUT_TEST, sizeof(PUT_TEST)));
    EXPECT_EQ(sizeof(PUT_TEST), InformationLength);
    
    // Clear the packet
    Sender.Clear();
    
    // EXAMPLE FROM THE DLMS WEBSITE
    //Sent frame :
    //7EA00A00020023219318717E
    //Comments :
    //7E// HDLC frame opening flag
    //A00A //frame type and length
    //0002 // destination address (server) upper HDLC address 0x0001
    //0023 // destination address (server) lower HDLC address 0x0011
    //21// source address (client) 0x10
    //93// frame type SNRM (Set Normal Response Mode) 
    // information field with HDLC parameters not present, defaults are proposed
    //1871// Frame check sequence
    //7E// HDLC frame closing flag
    Error = Sender.MakePacket(Packet::NO_SEGMENT,
        HDLCAddress(uint16_t(0x0001), uint16_t(0x0011)),
        HDLCAddress(0x10),
        HDLCControl(HDLCControl::SNRM));
    EXPECT_EQ(SUCCESS, Error);
    EXPECT_EQ(0,
        std::memcmp(PUT_SNRM, (const uint8_t *) Sender, sizeof(PUT_SNRM)));
    EXPECT_EQ(sizeof(PUT_SNRM), Sender.GetPacketLength());
    
    // Now let's read the exact same thing to see if we can parse it!
    EXPECT_EQ(Packet::NO_SEGMENT, Sender.GetSegmentation());
    EXPECT_TRUE(HDLCAddress(uint16_t(0x0001), uint16_t(0x0011)) == Sender.GetDestinationAddress());
    EXPECT_TRUE(HDLCAddress(0x10) == Sender.GetSourceAddress());
    EXPECT_EQ(HDLCControl(HDLCControl::SNRM), Sender.GetControl());
    EXPECT_EQ(0, Sender.GetInformationLength());   

}

TEST(HDLCPacket, MakeByByte) 
{
    Packet Receiver;
    
    // Now let's read the exact same thing to see if we can parse it!
    size_t InformationLength = 0;
    size_t PacketIndex = sizeof(PUT_TEST_PACKET);
    const uint8_t * pBytes = PUT_TEST_PACKET;
    HDLCErrorCode Error = NEED_MORE;
    while (PacketIndex--)
    {
        Error = Receiver.MakeByByte(*pBytes++);
        if (Error != NEED_MORE)
        {
            break;
        }
    }
    ASSERT_EQ(SUCCESS, Error);
    EXPECT_EQ(Packet::NO_SEGMENT, Receiver.GetSegmentation());
    EXPECT_TRUE(HDLCAddress(uint8_t(0x67), uint8_t(0x7f)) == Receiver.GetDestinationAddress());
    EXPECT_TRUE(HDLCAddress(0x66) == Receiver.GetSourceAddress());
    EXPECT_EQ(HDLCControl(HDLCControl::UI), Receiver.GetControl());
    EXPECT_EQ(sizeof(PUT_TEST), Receiver.GetInformationLength());   
    EXPECT_EQ(0, std::memcmp(Receiver.GetInformation(InformationLength), PUT_TEST, sizeof(PUT_TEST)));
    EXPECT_EQ(sizeof(PUT_TEST), InformationLength);
    
    
    // No information packet
    Receiver.Clear();
    InformationLength = 0;
    PacketIndex = sizeof(PUT_SNRM);
    pBytes = PUT_SNRM;
    Error = NEED_MORE;
    while (PacketIndex--)
    {
        Error = Receiver.MakeByByte(*pBytes++);
        if (Error != NEED_MORE)
        {
            break;
        }
    }
    EXPECT_EQ(Packet::NO_SEGMENT, Receiver.GetSegmentation());
    EXPECT_TRUE(HDLCAddress(uint16_t(0x0001), uint16_t(0x0011)) == Receiver.GetDestinationAddress());
    EXPECT_TRUE(HDLCAddress(0x10) == Receiver.GetSourceAddress());
    EXPECT_EQ(HDLCControl(HDLCControl::SNRM), Receiver.GetControl());
    EXPECT_EQ(0, Receiver.GetInformationLength());   

}

static const uint8_t IDENTIFY_TEST_PACKET[] = { 0x20 };
static const uint8_t IDENTIFY_TEST1_PACKET[] = { 'I' };
static const uint8_t IDENTIFY_RESPONSE_PACKET[] = { 0x00, 0x04, 0x01, 0x00 };

TEST(HDLCPacket, Identify) 
{
    Packet Identify;
    
    size_t InformationLength = 0;
    size_t PacketIndex = sizeof(IDENTIFY_TEST_PACKET);
    const uint8_t * pBytes = IDENTIFY_TEST_PACKET;
    HDLCErrorCode Error = NEED_MORE;
    while (PacketIndex--)
    {
        Error = Identify.MakeByByte(*pBytes++);
        if (Error != NEED_MORE)
        {
            break;
        }
    }
    ASSERT_EQ(SUCCESS, Error);
    EXPECT_EQ(Packet::NO_SEGMENT, Identify.GetSegmentation());
    EXPECT_TRUE(HDLCAddress() == Identify.GetDestinationAddress());
    EXPECT_TRUE(HDLCAddress() == Identify.GetSourceAddress());
    EXPECT_EQ(HDLCControl(HDLCControl::IDENT), Identify.GetControl());
    EXPECT_EQ(0, Identify.GetInformationLength());   
    EXPECT_EQ(nullptr, Identify.GetInformation(InformationLength));   

    Identify.Clear();
    InformationLength = 0;
    PacketIndex = sizeof(IDENTIFY_TEST1_PACKET);
    pBytes = IDENTIFY_TEST1_PACKET;
    while (PacketIndex--)
    {
        Error = Identify.MakeByByte(*pBytes++);
        if (Error != NEED_MORE)
        {
            break;
        }
    }
    ASSERT_EQ(SUCCESS, Error);
    EXPECT_EQ(Packet::NO_SEGMENT, Identify.GetSegmentation());
    EXPECT_TRUE(HDLCAddress() == Identify.GetDestinationAddress());
    EXPECT_TRUE(HDLCAddress() == Identify.GetSourceAddress());
    EXPECT_EQ(HDLCControl(HDLCControl::IDENT), Identify.GetControl());
    EXPECT_EQ(0, Identify.GetInformationLength());   
    EXPECT_EQ(nullptr, Identify.GetInformation(InformationLength));  
    
    Identify.Clear();
    InformationLength = 0;
    PacketIndex = sizeof(IDENTIFY_RESPONSE_PACKET);
    pBytes = IDENTIFY_RESPONSE_PACKET;
    while (PacketIndex--)
    {
        Error = Identify.MakeByByte(*pBytes++);
        if (Error != NEED_MORE)
        {
            break;
        }
    }
    ASSERT_EQ(SUCCESS, Error);
    EXPECT_EQ(Packet::NO_SEGMENT, Identify.GetSegmentation());
    EXPECT_TRUE(HDLCAddress() == Identify.GetDestinationAddress());
    EXPECT_TRUE(HDLCAddress() == Identify.GetSourceAddress());
    EXPECT_EQ(HDLCControl(HDLCControl::IDENTR), Identify.GetControl());
    EXPECT_EQ(sizeof(IDENTIFY_RESPONSE_PACKET), Identify.GetInformationLength()) ;   
    EXPECT_EQ(0, std::memcmp(Identify.GetInformation(InformationLength), IDENTIFY_RESPONSE_PACKET, 
        sizeof(IDENTIFY_RESPONSE_PACKET)));
      
}

