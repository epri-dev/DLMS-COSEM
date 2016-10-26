#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/hdlc/HDLCClient.cpp"
#include "DummySerial.h"

using namespace EPRI;

static DummySerial    TestSerial;

static const uint8_t PH_TEST1[] = { 0xE6, 0xE6, 0x00, 0x1D, 0x64, 0x00, 0x14, 0x00, 0x00 };

class HDLCClientFixture : public HDLCClient, public testing::Test
{
public:
    HDLCClientFixture()
        : HDLCClient(HDLCAddress(uint8_t(0x67), uint8_t(0x7f)), &TestSerial, HDLCOptions({ false, 3, 500 }), 10)
    {
    }
};

TEST_F(HDLCClientFixture, PacketHandler)
{
    // Get a working packet...
    //
    Packet * pPacket = this->GetWorkingRXPacket();
    ASSERT_NE(nullptr, pPacket);
    
    // Make a packet to pretend that we have received it...
    //
    HDLCErrorCode Error = pPacket->MakePacket(Packet::NO_SEGMENT,
        HDLCAddress(uint8_t(0x67), uint8_t(0x7f)),
        HDLCAddress(0x66),
        HDLCControl(HDLCControl::UI),
        PH_TEST1,
        sizeof(PH_TEST1));
    ASSERT_EQ(SUCCESS, Error);
    
    // Done with it, enqueue it for handling and dispatch...
    //
    this->EnqueueWorkingRXPacket();
    
    // Process it...
    //
    EXPECT_EQ(SUCCESS, this->ProcessPacketReception());

}