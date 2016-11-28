#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/APDU/GET-Response.cpp"

using namespace EPRI;

static const std::vector<uint8_t> FINAL = 
{ 
    0xC4, 0x01, 0x43, 0x00, 0x09, 0x0C, 0x07, 0xE0, 0x01, 0x04, 0x01, 0x09, 0x32, 0x0A, 0x00, 0x80, 
    0x00, 0x00
};

TEST(GET_Response, GeneralUsage) 
{
    Get_Response_Normal Response;
    DLMSVector          Data(FINAL);
    
    ASSERT_TRUE(Response.Parse(1, 1, &Data));
    ASSERT_EQ(0x43, Response.invoke_id_and_priority);
    ASSERT_EQ(Get_Response::data, Response.result.which());
    ASSERT_EQ(DLMSVector({0x09, 0x0C, 0x07, 0xE0, 0x01, 0x04, 0x01, 0x09, 0x32, 0x0A, 0x00, 0x80, 
                          0x00, 0x00}), 
              Response.result.get<DLMSVector>());
    
    ASSERT_EQ(FINAL, Response.GetBytes());
    
}