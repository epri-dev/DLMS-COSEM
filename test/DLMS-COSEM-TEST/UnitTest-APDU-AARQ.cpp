#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/APDU/AARQ.cpp"

using namespace EPRI;

TEST(AARQ, GeneralUsage) 
{
    AARQ a1;
  
    ASSERT_TRUE(a1.application_context_name.Append(ASNObjectIdentifier({ 2, 16, 756, 5, 8, 1, 1 })));
    //
    // Just the application_context_name does not make a valid AARQ...
    //
    ASSERT_FALSE(a1.IsValid());
       
    std::vector<uint8_t> A1CHECK_CONTEXT_NAME = 
    { 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 
        0x74, 0x05, 0x08, 0x01, 0x01 };
    ASSERT_TRUE(a1.application_context_name == A1CHECK_CONTEXT_NAME);
    
    a1.sender_acse_requirements.Append(
        ASNBitString(a1.sender_acse_requirements.GetCurrentSchemaTypeMaxLength(), 1));
    std::vector<uint8_t> A1CHECK_ACSE_REQ = { 0x8A, 0x02, 0x07, 0x80 };
    ASSERT_TRUE(a1.sender_acse_requirements == A1CHECK_ACSE_REQ);
    
    ASSERT_TRUE(a1.mechanism_name.Append(ASNObjectIdentifier({ 2, 16, 756, 5, 8, 2, 1 })));
    std::vector<uint8_t> A1CHECK_MECHANISM_NAME = 
    { 0x8B, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08,  0x02, 0x01 };
    ASSERT_TRUE(a1.mechanism_name == A1CHECK_MECHANISM_NAME);
 
    ASSERT_TRUE(a1.calling_authentication_value.Append(ASNType(ASN::GraphicString, std::string("33333333"))));
    std::vector<uint8_t> A1CHECK_AUTHENTICATION_VALUE = { 0xAC, 0x0A, 0x80, 0x08, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33 };
    ASSERT_TRUE(a1.calling_authentication_value == A1CHECK_AUTHENTICATION_VALUE);
        
    std::vector<uint8_t> FINAL = 
    { 
        0x60, 0x36, 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 
        0x74, 0x05, 0x08, 0x01, 0x01, 0x8A, 0x02, 0x07, 0x80, 0x8B, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 
        0x02, 0x01, 0xAC, 0x0A, 0x80, 0x08, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0x33, 0xBE, 0x10, 
        0x04, 0x0E, 0x01, 0x00, 0x00, 0x00, 0x06, 0x5F, 0x1F, 0x04, 0x00, 0x00, 0x7E, 0x1F, 0x00, 0x00, 
    };
 
    ASSERT_TRUE(a1.user_information.Append(
        ASNType(ASN::OCTET_STRING, 
        std::vector<uint8_t>({0x01, 0x00, 0x00, 0x00, 0x06, 0x5F, 0x1F, 0x04, 0x00, 0x00, 0x7E, 0x1F, 0x00, 0x00}))));
    std::vector<uint8_t> AARQ_VEC = a1.GetBytes();
    ASSERT_TRUE(AARQ_VEC == FINAL);
    
}
