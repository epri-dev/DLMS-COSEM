#include <gtest/gtest.h>
#include <string>

#include "../../lib/DLMS-COSEM/include/hdlc/HDLCAddress.h"

using namespace EPRI;

TEST(HDLCAddress, ConstructorsAndComparisons) 
{
    HDLCAddress hdlc1(0x01);
    uint8_t hdlc1_EXPECTED[] = { 0x03 };
    EXPECT_EQ(0, std::memcmp(&hdlc1, hdlc1_EXPECTED, sizeof(hdlc1_EXPECTED)));
    
    HDLCAddress hdlc2(uint8_t(0x67), uint8_t(0x7f));
    uint8_t hdlc2_EXPECTED[] = { 0xCE, 0xFF };
    EXPECT_EQ(0, std::memcmp(&hdlc2, hdlc2_EXPECTED, sizeof(hdlc2_EXPECTED)));
    
    HDLCAddress hdlc3(uint16_t(0x1234), uint16_t(0x3FFF));
    uint8_t hdlc3_EXPECTED[] = { 0x48, 0x68, 0xFE, 0xFF };
    EXPECT_EQ(0, std::memcmp(&hdlc3, hdlc3_EXPECTED, sizeof(hdlc3_EXPECTED)));
    
    EXPECT_FALSE(hdlc1 == hdlc2);
    hdlc1 = hdlc2;
    EXPECT_TRUE(hdlc1 == hdlc2);

}
