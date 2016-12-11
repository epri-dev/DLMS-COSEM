#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/APDU/ACTION-Response.cpp"

using namespace EPRI;

//<action-response-normal>
//  <invoke-id-and-priority>
//    <invoke-id>1</invoke-id>
//    <is-priority>True</is-priority>
//    <requires-confirmation>False</requires-confirmation>
//  </invoke-id-and-priority>
//  <response>
//    <action-result-code raw="0">Success</action-result-code>
//  </response>
//</action-response-normal>
static const std::vector<uint8_t> FINAL = 
{ 
    0xC7, 0x01, 0x81, 0x00, 0x00
};

TEST(ACTION_Response, GeneralUsage) 
{
    Action_Response_Normal Response;
    DLMSVector             Data(FINAL);
    
    ASSERT_TRUE(Response.Parse(1, 1, &Data));
    ASSERT_EQ(0x81, Response.invoke_id_and_priority);
    ASSERT_EQ(Response.single_response.result, APDUConstants::Action_Result::success);
    ASSERT_FALSE((bool) Response.single_response.return_parameters);
    
    ASSERT_EQ(FINAL, Response.GetBytes());
    
}