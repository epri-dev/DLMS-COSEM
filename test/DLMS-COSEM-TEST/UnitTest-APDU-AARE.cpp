#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/APDU/AARE.cpp"

using namespace EPRI;

static const std::vector<uint8_t> FINAL = 
{ 
    0x61, 0x29, 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01, 0xA2, 0x03, 0x02,
    0x01, 0x00, 0xA3, 0x05, 0xA1, 0x03, 0x02, 0x01, 0x00, 0xBE, 0x10, 0x04, 0x0E, 0x08, 0x00, 0x06,
    0x5F, 0x1F, 0x04, 0x00, 0x00, 0x38, 0x1F, 0x00, 0x9B, 0x00, 0x07
};


TEST(AARE, Build) 
{
    AARE                a1;
    ASNObjectIdentifier ApplicationContext({ 2, 16, 756, 5, 8, 1, 1 });
    
    ASSERT_TRUE(a1.application_context_name.Append(ApplicationContext));
    //
    // Just the application_context_name does not make a valid AARE...
    //
    ASSERT_FALSE(a1.IsValid());
       
    std::vector<uint8_t> A1CHECK_CONTEXT_NAME = 
    { 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85, 
        0x74, 0x05, 0x08, 0x01, 0x01 };
    ASSERT_TRUE(a1.application_context_name == A1CHECK_CONTEXT_NAME);

    ASSERT_TRUE(a1.result.Append(int8_t(AARE::AssociationResult::accepted)));
    std::vector<uint8_t> A1CHECK_RESULT = 
        { 0xA2, 0x03, 0x02, 0x01, 0x00 };
    ASSERT_TRUE(a1.result == A1CHECK_RESULT);
    
    ASSERT_TRUE(a1.result_source_diagnostic.SelectChoice(AARE::AssociateDiagnosticChoice::acse_service_user));
    ASSERT_TRUE(a1.result_source_diagnostic.Append(int8_t(AARE::AssociateDiagnosticUser::user_null)));
    std::vector<uint8_t> A1CHECK_DIAGNOSTIC = 
    { 0xA3, 0x05, 0xA1, 0x03, 0x02, 0x01, 0x00 };
    ASSERT_TRUE(a1.result_source_diagnostic == A1CHECK_DIAGNOSTIC);
 
    ASSERT_TRUE(a1.user_information.Append(DLMSVector({ 0x08, 0x00, 0x06, 0x5F, 0x1F, 0x04, 
                                                        0x00, 0x00, 0x38, 0x1F, 0x00, 0x9B, 0x00, 0x07 })));
    std::vector<uint8_t> AARE_VEC = a1.GetBytes();
    ASSERT_TRUE(AARE_VEC == FINAL);
    
}

TEST(AARE, Parse) 
{
    AARE        a1;
    DLMSVector  Data(FINAL);
    
    ASSERT_TRUE(a1.Parse(&Data, 1, 1));
    //
    // Let's validate what we have!
    //
    ASNObjectIdentifier ApplicationContext({ 2, 16, 756, 5, 8, 1, 1 });
    ASNType             Result(ASN::INTEGER, int8_t(AARE::AssociationResult::accepted));
    ASNType             Diagnostic(ASN::INTEGER, int8_t(AARE::AssociateDiagnosticUser::user_null));
    ASNType             UserInformation(ASN::OCTET_STRING, 
                                        DLMSVector({ 0x08, 0x00, 0x06, 0x5F, 0x1F, 0x04, 
                                                     0x00, 0x00, 0x38, 0x1F, 0x00, 0x9B, 0x00, 0x07 }));
    
    ASNType Current;
    ASSERT_EQ(ASNType::GetNextResult::VALUE_RETRIEVED, a1.application_context_name.GetNextValue(&Current));
    ASSERT_TRUE(ApplicationContext == Current);
    
    DLMSValue Value1;
    ASSERT_EQ(ASNType::GetNextResult::VALUE_RETRIEVED, a1.result.GetNextValue(&Value1));
    ASSERT_FALSE(IsSequence(Value1));
    ASSERT_EQ(DLMSValueGet<int8_t>(Value1), int8_t(AARE::AssociationResult::accepted));

    
    //    ASN_BEGIN_SCHEMA(AARE::Associate_Source_Diagnostic_Schema)
    //        ASN_BEGIN_CHOICE
    //            ASN_BEGIN_CHOICE_ENTRY(acse_service_user)
    //                ASN_INTEGER_LIST_TYPE
    //                (   
    //                    ASN::CONSTRUCTED, 
    //                    { 
    //                        user_null, 
    //                        user_no_reason_given, 
    //                        application_context_name_not_supported,
    //                        authentication_mechanism_name_not_recognized,
    //                        authentication_mechanism_name_required,
    //                        authentication_failure,
    //                        authentication_required,
    //                    }
    //                )
    //            ASN_END_CHOICE_ENTRY
    //            ASN_BEGIN_CHOICE_ENTRY(acse_service_provider)
    //                ASN_INTEGER_LIST_TYPE
    //                (   
    //                    ASN::CONSTRUCTED, 
    //                    { 
    //                        provider_null, 
    //                        provider_no_reason_given, 
    //                        no_common_acse_version,
    //                    }
    //                )
    //            ASN_END_CHOICE_ENTRY
    //        ASN_END_CHOICE
    //    ASN_END_SCHEMA

    // Loop until we get a value... or fail...
    //
    int8_t Choice;
    ASSERT_EQ(ASNType::GetNextResult::VALUE_RETRIEVED, a1.result_source_diagnostic.GetNextValue(&Value1));
    ASSERT_TRUE(a1.result_source_diagnostic.GetChoice(&Choice));
    ASSERT_EQ(AARE::AssociateDiagnosticChoice::acse_service_user, Choice);
    ASSERT_EQ(DLMSValueGet<int8_t>(Value1), int8_t(AARE::AssociateDiagnosticUser::user_null));
    //
    // We should be at the end of the schema...
    //
    ASSERT_EQ(ASNType::GetNextResult::END_OF_SCHEMA, a1.result_source_diagnostic.GetNextValue(&Value1));
    a1.result_source_diagnostic.Rewind();
    ASSERT_FALSE(a1.result_source_diagnostic.GetChoice(&Choice));
    //
    // After Rewind, we should be able to get the value again...
    //
    ASSERT_EQ(ASNType::GetNextResult::VALUE_RETRIEVED, a1.result_source_diagnostic.GetNextValue(&Value1));
    ASSERT_TRUE(a1.result_source_diagnostic.GetChoice(&Choice));
    ASSERT_EQ(AARE::AssociateDiagnosticChoice::acse_service_user, Choice);
    ASSERT_EQ(DLMSValueGet<int8_t>(Value1), int8_t(AARE::AssociateDiagnosticUser::user_null));
   
    ASSERT_EQ(ASNType::GetNextResult::VALUE_RETRIEVED, a1.user_information.GetNextValue(&Current));
    ASSERT_TRUE(UserInformation == Current);
    

}
