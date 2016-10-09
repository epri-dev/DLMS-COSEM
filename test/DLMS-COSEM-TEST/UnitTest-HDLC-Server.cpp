#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/hdlc/HDLCServer.cpp"
#include "DummySerial.h"

using namespace EPRI;

static DummySerial    TestSerial;


class HDLCServerFixture : public HDLCServer, public testing::Test
{
public:
    HDLCServerFixture()
        : HDLCServer(HDLCAddress(0x01), &TestSerial, HDLCOptions({ 3 }), 10)
    {
    }
};

TEST_F(HDLCServerFixture, Test)
{
}