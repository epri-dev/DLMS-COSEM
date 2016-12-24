#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/APDU/RLRQ.cpp"

using namespace EPRI;

static const std::vector<uint8_t> FINAL = 
{ 
    0x62, 0x00, 
};
    
TEST(RLRQ, GeneralUsage) 
{
    RLRQ r1;
    
//    ASSERT_TRUE(r1.reason.Append(int8_t(EPRI::RLRQ::ReleaseRequestReason::normal))); 
//    //
//    // Just the application_context_name does not make a valid RLRQ...
//    //
//    ASSERT_FALSE(r1.IsValid());
//    std::vector<uint8_t> R1CHECK_REASON = { 0x00 };
//    ASSERT_TRUE(r1.reason == R1CHECK_REASON);
       
    std::vector<uint8_t> RLRQ_VEC = r1.GetBytes();
    ASSERT_TRUE(RLRQ_VEC == FINAL);
    
}

TEST(RLRQ, Parse) 
{
    RLRQ        r1;
    DLMSVector  Data(FINAL);
    
    ASSERT_TRUE(r1.Parse(&Data,1, 1)); 

    DLMSValue   Value1;
    ASSERT_EQ(ASNType::GetNextResult::VALUE_EMPTY, r1.reason.GetNextValue(&Value1));
//    ASSERT_EQ(DLMSValueGet<int8_t>(Value1), EPRI::RLRQ::ReleaseRequestReason::normal);
}
