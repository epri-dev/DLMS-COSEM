#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/APDU/AARQ.cpp"

using namespace EPRI;

static const std::vector<uint8_t> FINAL = 
{ 
    0x60, 0x36, 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 
    0x74, 0x05, 0x08, 0x01, 0x01, 0x8A, 0x02, 0x07, 0x80, 0x8B, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 
    0x02, 0x01, 0xAC, 0x0A, 0x80, 0x08, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0xBE, 0x10, 
    0x04, 0x0E, 0x01, 0x00, 0x00, 0x00, 0x06, 0x5F, 0x1F, 0x04, 0x00, 0x00, 0x7E, 0x1F, 0x00, 0x00, 
};
    
TEST(AARQ, GeneralUsage) 
{
    AARQ a1;
    ASNObjectIdentifier ApplicationContext({ 2, 16, 756, 5, 8, 1, 1 });
    ASNObjectIdentifier MechanismName({ 2, 16, 756, 5, 8, 2, 1 }, ASN::IMPLICIT);

    ASSERT_TRUE(a1.application_context_name.Append(ApplicationContext));
    //
    // Just the application_context_name does not make a valid AARQ...
    //
    ASSERT_FALSE(a1.IsValid());
       
    std::vector<uint8_t> A1CHECK_CONTEXT_NAME = 
    { 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 
        0x74, 0x05, 0x08, 0x01, 0x01 };
    ASSERT_TRUE(a1.application_context_name == A1CHECK_CONTEXT_NAME);
    
    ASNBitString        ACSERequirements(a1.sender_acse_requirements.GetCurrentSchemaTypeSize(), 1);
    ASSERT_TRUE(a1.sender_acse_requirements.Append(ACSERequirements));
    std::vector<uint8_t> A1CHECK_ACSE_REQ = { 0x8A, 0x02, 0x07, 0x80 };
    ASSERT_TRUE(a1.sender_acse_requirements == A1CHECK_ACSE_REQ);
    
    ASSERT_TRUE(a1.mechanism_name.Append(MechanismName));
    std::vector<uint8_t> A1CHECK_MECHANISM_NAME = 
    { 0x8B, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08,  0x02, 0x01 };
    ASSERT_TRUE(a1.mechanism_name == A1CHECK_MECHANISM_NAME);
 
    ASSERT_TRUE(a1.calling_authentication_value.SelectChoice(APDUConstants::AuthenticationValueChoice::charstring));
    ASSERT_TRUE(a1.calling_authentication_value.Append(std::string("33333333")));
    std::vector<uint8_t> A1CHECK_AUTHENTICATION_VALUE = { 0xAC, 0x0A, 0x80, 0x08, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33 };
    ASSERT_TRUE(a1.calling_authentication_value == A1CHECK_AUTHENTICATION_VALUE);
 
    ASSERT_TRUE(a1.user_information.Append(DLMSVector({ 0x01, 0x00, 0x00, 0x00, 0x06, 0x5F, 
                                                        0x1F, 0x04, 0x00, 0x00, 0x7E, 0x1F, 0x00, 0x00 })));
    std::vector<uint8_t> AARQ_VEC = a1.GetBytes();
    ASSERT_TRUE(AARQ_VEC == FINAL);
    
}

TEST(AARQ, Parse) 
{
    AARQ        a1;
    DLMSVector  Data(FINAL);
    
    ASSERT_TRUE(a1.Parse(&Data));

    ASNObjectIdentifier ApplicationContext({ 2, 16, 756, 5, 8, 1, 1 });
    ASNObjectIdentifier MechanismName({ 2, 16, 756, 5, 8, 2, 1 }, ASN::IMPLICIT);
    ASNType             UserInformation(ASN::OCTET_STRING, 
                            DLMSVector({ 0x01, 0x00, 0x00, 0x00, 0x06, 0x5F, 
                                            0x1F, 0x04, 0x00, 0x00, 0x7E, 0x1F, 0x00, 0x00 }));
    ASNType             Current;
    ASSERT_EQ(ASNType::GetNextResult::VALUE_RETRIEVED, a1.application_context_name.GetNextValue(&Current));
    ASSERT_TRUE(ApplicationContext == Current);
    
    ASNBitString        ACSERequirements(a1.sender_acse_requirements.GetCurrentSchemaTypeSize(), 1);
    ASNBitString        BitString(a1.sender_acse_requirements.GetCurrentSchemaTypeSize());
    ASSERT_EQ(ASNType::GetNextResult::VALUE_RETRIEVED, a1.sender_acse_requirements.GetNextValue(&BitString));
    ASSERT_TRUE(ACSERequirements == BitString);

    ASSERT_EQ(ASNType::GetNextResult::VALUE_RETRIEVED, a1.mechanism_name.GetNextValue(&Current));
    ASSERT_TRUE(MechanismName == Current);

    int8_t              Choice;
    DLMSValue           Value1;
    ASSERT_EQ(ASNType::GetNextResult::VALUE_RETRIEVED, a1.calling_authentication_value.GetNextValue(&Value1));
    ASSERT_TRUE(a1.calling_authentication_value.GetChoice(&Choice));
    ASSERT_EQ(APDUConstants::AuthenticationValueChoice::charstring, Choice);
    ASSERT_FALSE(IsSequence(Value1));
    ASSERT_EQ(DLMSValueGet<std::string>(Value1), std::string("33333333"));
 
    ASSERT_EQ(ASNType::GetNextResult::VALUE_RETRIEVED, a1.user_information.GetNextValue(&Current));
    ASSERT_TRUE(UserInformation == Current);

}
