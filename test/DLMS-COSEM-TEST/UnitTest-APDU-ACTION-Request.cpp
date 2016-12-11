#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/APDU/ACTION-Request.cpp"

using namespace EPRI;

//<action-request-normal>
//  <invoke-id-and-priority>
//    <invoke-id>1</invoke-id>
//    <is-priority>True</is-priority>
//    <requires-confirmation>False</requires-confirmation>
//  </invoke-id-and-priority>
//  <cosem-method-descriptor>
//    <class-id>9</class-id>
//    <instance-id raw="00 00 0a 00 00 ff">0-0:10.0.0.255</instance-id>
//    <method-id>1</method-id>
//  </cosem-method-descriptor>
//  <method-invocation-parameters>
//    <long-unsigned>2</long-unsigned>
//  </method-invocation-parameters>
//</action-request-normal>
static const std::vector<uint8_t> FINAL = 
{ 
    0xC3, 0x01, 0x81, 0x00, 0x09, 0x00, 0x00, 0x0A, 
    0x00, 0x00, 0xFF, 0x01, 0x01, 0x12, 0x00, 0x02
};

//<action-request-normal>
//  <invoke-id-and-priority>
//    <invoke-id>1</invoke-id>
//    <is-priority>True</is-priority>
//    <requires-confirmation>False</requires-confirmation>
//  </invoke-id-and-priority>
//  <cosem-method-descriptor>
//    <class-id>9</class-id>
//    <instance-id raw="00 00 0a 00 00 ff">0-0:10.0.0.255</instance-id>
//    <method-id>1</method-id>
//  </cosem-method-descriptor>
//</action-request-normal>
static const std::vector<uint8_t> FINAL1 = 
{ 
    0xC3, 0x01, 0x81, 0x09, 0x09, 0x01, 0x10, 0x0A, 
    0x42, 0x34, 0xFF, 0x01, 0x00
};

TEST(ACTION_Request, GeneralUsage) 
{
    Action_Request_Normal Request;
    DLMSVector            Data(FINAL);
    
    ASSERT_TRUE(Request.Parse(1, 1, &Data));
    ASSERT_EQ(0x81, Request.invoke_id_and_priority);
    ASSERT_EQ(0x0009, Request.cosem_method_descriptor.class_id);
    ASSERT_EQ(DLMSVector({0x00, 0x00, 0x0A, 0x00, 0x00, 0xFF}), Request.cosem_method_descriptor.instance_id);
    ASSERT_EQ(0x01, Request.cosem_method_descriptor.method_id);
    ASSERT_TRUE((bool)Request.method_invocation_parameters);
    ASSERT_EQ(DLMSVector({ 0x01, 0x12, 0x00, 0x02 }),
              Request.method_invocation_parameters.value());
    
    ASSERT_EQ(FINAL, Request.GetBytes());
    Request.cosem_method_descriptor.method_id = 4;
    ASSERT_EQ(
        std::vector<uint8_t>({ 0xC3, 0x01, 0x81, 0x00, 0x09, 0x00, 0x00, 0x0A, 
                               0x00, 0x00, 0xFF, 0x04, 0x01, 0x12, 0x00, 0x02 }),
        Request.GetBytes());
    

    DLMSVector            Data1(FINAL1);
    Request.Clear();
    ASSERT_TRUE(Request.Parse(1, 1, &Data1));
    ASSERT_EQ(0x81, Request.invoke_id_and_priority);
    ASSERT_EQ(0x0909, Request.cosem_method_descriptor.class_id);
    ASSERT_EQ(DLMSVector({ 0x01, 0x10, 0x0A, 0x42, 0x34, 0xFF }), Request.cosem_method_descriptor.instance_id);
    ASSERT_EQ(0x01, Request.cosem_method_descriptor.method_id);
    ASSERT_FALSE((bool)Request.method_invocation_parameters);
    
    ASSERT_EQ(FINAL1, Request.GetBytes());
    
}