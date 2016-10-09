#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/hdlc/HDLCStatistics.cpp"
#include "../../lib/DLMS-COSEM/hdlc/HDLCMAC.cpp"
#include "DummySerial.h"

using namespace EPRI;

static DummySerial    TestSerial;

TEST(HDLCMAC, Constructor) 
{
}