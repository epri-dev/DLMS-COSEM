#include <gtest/gtest.h>
#include <string>

#include "../../lib/DLMS-COSEM/COSEMObjectInstanceID.cpp"

using namespace EPRI;

TEST(COSEMObjectInstanceID, Constructors) 
{
    // Empty
    COSEMObjectInstanceID  ID1;
    ASSERT_TRUE(ID1.IsEmpty());
    
    DLMSVector             Vector1({ 1, 2, 3, 4, 5, 6 });
    ASSERT_TRUE(ID1.Parse(&Vector1));
    ASSERT_EQ("1-2:3.4.5*6", ID1.ToString());
    
    COSEMObjectInstanceID  ID2({ 3, 4, 5, 6, 7, 8 });
    ASSERT_EQ("3-4:5.6.7*8", ID2.ToString());
    ASSERT_FALSE(ID2.IsEmpty());
   
        
    ASSERT_EQ(3, ID2.GetValueGroup(COSEMObjectInstanceID::VALUE_GROUP_A));
    ASSERT_EQ(4, ID2.GetValueGroup(COSEMObjectInstanceID::VALUE_GROUP_B));
    ASSERT_EQ(5, ID2.GetValueGroup(COSEMObjectInstanceID::VALUE_GROUP_C));
    ASSERT_EQ(6, ID2.GetValueGroup(COSEMObjectInstanceID::VALUE_GROUP_D));
    ASSERT_EQ(7, ID2.GetValueGroup(COSEMObjectInstanceID::VALUE_GROUP_E));
    ASSERT_EQ(8, ID2.GetValueGroup(COSEMObjectInstanceID::VALUE_GROUP_F));

    ASSERT_NE(ID2, ID1);
    ASSERT_EQ(ID2, COSEMObjectInstanceID({ 3, 4, 5, 6, 7, 8 }));

    DLMSVector Vector2 = ID2;
    ASSERT_EQ(Vector2, DLMSVector({ 3, 4, 5, 6, 7, 8 }));
    
}
