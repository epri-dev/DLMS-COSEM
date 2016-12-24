#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/APDU/xDLMS.cpp"

using namespace EPRI;

TEST(xDLMS, Constructors) 
{
    xDLMS::InitiateRequest IRQ1;
    ASSERT_FALSE(IRQ1.Initialized());
    
    xDLMS::InitiateRequest IRQ2(42,
        xDLMS::AvailableStackConformance);
    ASSERT_TRUE(IRQ2.Initialized());
    ASSERT_EQ(IRQ2.APDUSize(), 42);
    ASSERT_EQ(IRQ2.ConformanceBits(), xDLMS::AvailableStackConformance);
    ASSERT_EQ(IRQ2.DLMSVersion(), APDUConstants::CURRENT_DLMS_VERSION);
    ASSERT_TRUE(IsBlank(IRQ2.DedicatedKey())); 
    ASSERT_TRUE(IsBlank(IRQ2.QOS())); 
    
    std::vector<uint8_t> IRQ2Bytes = IRQ2.GetBytes();   
    std::vector<uint8_t> IRQ2Expected( { 0x01, 0x00, 0x00, 0x00, 0x06, 0x5F, 0x1F, 
            0x04, 0x00, 0x00, 0x00, 0x19, 0x00, 0x2A } );
    ASSERT_EQ(IRQ2Expected, IRQ2Bytes);
    
    DLMSVector ParseBytes(IRQ2Expected);
    IRQ2.Clear();  
    ASSERT_FALSE(IRQ2.Initialized()); 
    ASSERT_TRUE(IRQ2.Parse(&ParseBytes)); 
    ASSERT_TRUE(IRQ2.Initialized());
    ASSERT_EQ(IRQ2.APDUSize(), 42);
    ASSERT_EQ(IRQ2.ConformanceBits(), xDLMS::AvailableStackConformance);
    ASSERT_EQ(IRQ2.DLMSVersion(), APDUConstants::CURRENT_DLMS_VERSION);
    ASSERT_TRUE(IsBlank(IRQ2.DedicatedKey())); 
    ASSERT_TRUE(IsBlank(IRQ2.QOS())); 
     
    xDLMS::InitiateResponse IRE1;
    ASSERT_FALSE(IRE1.Initialized()); 
    
    xDLMS::InitiateResponse IRE2(640);
    ASSERT_TRUE(IRE2.Initialized());
    ASSERT_EQ(IRE2.APDUSize(), 640);
    ASSERT_EQ(IRE2.ConformanceBits(), xDLMS::AvailableStackConformance);
    ASSERT_EQ(IRE2.DLMSVersion(), APDUConstants::CURRENT_DLMS_VERSION);
    ASSERT_TRUE(IsBlank(IRE2.QOS())); 
    
    std::vector<uint8_t> IRE2Bytes = IRE2.GetBytes();;
    std::vector<uint8_t> IRE2Expected( { 0x08, 0x00, 0x06, 0x5F, 0x1F, 
            0x04, 0x00, 0x00, 0x00, 0x19, 0x02, 0x80, 0x00, 0x07 } );
    ASSERT_EQ(IRE2Expected, IRE2Bytes);
    
    ParseBytes = IRE2Expected;
    IRE2.Clear();  
    ASSERT_FALSE(IRE2.Initialized()); 
    ASSERT_TRUE(IRE2.Parse(&ParseBytes)); 
    ASSERT_TRUE(IRE2.Initialized());
    ASSERT_EQ(IRE2.APDUSize(), 640);
    ASSERT_EQ(IRE2.ConformanceBits(), xDLMS::AvailableStackConformance);
    ASSERT_EQ(IRE2.DLMSVersion(), APDUConstants::CURRENT_DLMS_VERSION);
    ASSERT_TRUE(IsBlank(IRE2.QOS())); 
    
    
}