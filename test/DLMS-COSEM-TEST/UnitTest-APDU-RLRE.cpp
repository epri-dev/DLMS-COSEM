#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/APDU/RLRE.cpp"

using namespace EPRI;

static const std::vector<uint8_t> FINAL = 
{ 
    0x63, 0x15, 0x80, 0x01, 0x00, 0xBE, 0x10, 0x04, 0x0E, 0x08, 0x00, 0x06, 0x5F, 0x1F, 0x04, 0x00,
    0x00, 0x38, 0x1F, 0x00, 0x9B, 0x00, 0x07
};

TEST(RLRE, Build) 
{
    RLRE r1;
    
    ASSERT_TRUE(r1.reason.Append(int8_t(EPRI::RLRE::ReleaseResponseReason::normal))); 
    //
    // Just the application_context_name does not make a valid RLRQ...
    //
    ASSERT_FALSE(r1.IsValid());
    std::vector<uint8_t> R1CHECK_REASON = { 0x80, 0x01, 0x00 };
    ASSERT_TRUE(r1.reason == R1CHECK_REASON);
    
    ASSERT_TRUE(r1.user_information.Append(DLMSVector({ 0x08, 0x00, 0x06, 0x5F, 0x1F, 0x04, 0x00,
                                                        0x00, 0x38, 0x1F, 0x00, 0x9B, 0x00, 0x07 })));
    std::vector<uint8_t> RLRE_VEC = r1.GetBytes();
    ASSERT_TRUE(RLRE_VEC == FINAL);
    
}

TEST(RLRE, Parse) 
{
    RLRE        r1;
    DLMSVector  Data(FINAL);
    ASNType             UserInformation(ASN::OCTET_STRING, 
                                        DLMSVector({ 0x08, 0x00, 0x06, 0x5F, 0x1F, 0x04, 0x00,
                                                     0x00, 0x38, 0x1F, 0x00, 0x9B, 0x00, 0x07 }));
    
    ASSERT_TRUE(r1.Parse(&Data, 1, 1));

    DLMSValue   Value1;
    ASSERT_EQ(ASNType::GetNextResult::VALUE_RETRIEVED, r1.reason.GetNextValue(&Value1));
    ASSERT_EQ(DLMSValueGet<int8_t>(Value1), EPRI::RLRE::ReleaseResponseReason::normal); 
    
    ASNType Current;
    ASSERT_EQ(ASNType::GetNextResult::VALUE_RETRIEVED, r1.user_information.GetNextValue(&Current));
    ASSERT_TRUE(UserInformation == Current);

}
