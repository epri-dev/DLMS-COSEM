#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/APDU/GET-Request.cpp"

using namespace EPRI;

static const std::vector<uint8_t> FINAL = 
{ 
    0xC0, 0x01, 0x45, 0x00, 0x07, 0x00, 0x00, 0x63, 
    0x62, 0x01, 0xFF, 0x02, 0x00
};

TEST(GET_Request, GeneralUsage) 
{
    Get_Request_Normal Request;
    DLMSVector         Data(FINAL);
    
    ASSERT_TRUE(Request.Parse(1, 1, &Data));
    ASSERT_EQ(0x45, Request.invoke_id_and_priority);
    ASSERT_EQ(0x0007, Request.cosem_attribute_descriptor.class_id);
    ASSERT_EQ(DLMSVector({0x00, 0x00, 0x63, 0x62, 0x01, 0xFF}), Request.cosem_attribute_descriptor.instance_id);
    ASSERT_EQ(0x02, Request.cosem_attribute_descriptor.attribute_id);
    
    ASSERT_EQ(FINAL, Request.GetBytes());
    Request.cosem_attribute_descriptor.attribute_id = 4;
    ASSERT_EQ(
        std::vector<uint8_t>({ 0xC0, 0x01, 0x45, 0x00, 0x07, 0x00, 0x00, 0x63, 0x62, 0x01, 0xFF, 0x04, 0x00 }),
        Request.GetBytes());
}