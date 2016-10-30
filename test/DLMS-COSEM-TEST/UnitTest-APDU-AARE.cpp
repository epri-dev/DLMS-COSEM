#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/APDU/AARE.cpp"

using namespace EPRI;

TEST(AARE, GeneralUsage) 
{
    AARE a1;
  
    ASSERT_TRUE(a1.application_context_name.Append(ASNObjectIdentifier({ 2, 16, 756, 5, 8, 1, 1 })));
    //
    // Just the application_context_name does not make a valid AARE...
    //
    ASSERT_FALSE(a1.IsValid());
       
    std::vector<uint8_t> A1CHECK_CONTEXT_NAME = 
    { 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 
        0x74, 0x05, 0x08, 0x01, 0x01 };
    ASSERT_TRUE(a1.application_context_name == A1CHECK_CONTEXT_NAME);

    ASSERT_TRUE(a1.result.Append(ASNType(ASN::INTEGER, int32_t(AARE::AssociationResult::accepted))));
    std::vector<uint8_t> A1CHECK_RESULT = 
        { 0xA2, 0x03, 0x02, 0x01, 0x00 };
    ASSERT_TRUE(a1.result == A1CHECK_RESULT);
    
    ASSERT_TRUE(a1.result_source_diagnostic.Append(ASNType(ASN::INTEGER, DLMSVariant(0))));
    std::vector<uint8_t> A1CHECK_DIAGNOSTIC = 
    { 0xA3, 0x05, 0xA1, 0x03, 0x02, 0x01, 0x00 };
    ASSERT_TRUE(a1.result_source_diagnostic == A1CHECK_DIAGNOSTIC);
 
    std::vector<uint8_t> FINAL = 
    { 
        0x61, 0x29, 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0xA2, 0x03, 0x02,
        0x01, 0x00, 0xA3, 0x05, 0xA1, 0x03, 0x02, 0x01, 0x00, 0xBE, 0x10, 0x04, 0x0E, 0x08, 0x00, 0x06,
        0x5F, 0x1F, 0x04, 0x00, 0x00, 0x38, 0x1F, 0x00, 0x9B, 0x00, 0x07
    };
 
    ASSERT_TRUE(a1.user_information.Append(
        ASNType(ASN::OCTET_STRING, 
        DLMSVector({0x08, 0x00, 0x06, 0x5F, 0x1F, 0x04, 0x00, 0x00, 0x38, 0x1F, 0x00, 0x9B, 0x00, 0x07}))));
    std::vector<uint8_t> AARE_VEC = a1.GetBytes();
    ASSERT_TRUE(AARE_VEC == FINAL);
    
}
