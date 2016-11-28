#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/interfaces/COSEMData.cpp"

using namespace EPRI;

// Use Cases
// octet-string
COSEM_BEGIN_SCHEMA(Octet_String_Schema_Test)
    COSEM_OCTET_STRING_TYPE
COSEM_END_SCHEMA
// CHOICE
COSEM_BEGIN_SCHEMA(Choice_Schema_Test1)
    COSEM_BEGIN_CHOICE
        COSEM_NULL_DATA_TYPE
        COSEM_BIT_STRING_TYPE
        COSEM_DOUBLE_LONG_TYPE
        COSEM_DOUBLE_LONG_UNSIGNED_TYPE
        COSEM_OCTET_STRING_TYPE
        COSEM_VISIBLE_STRING_TYPE
        COSEM_UTF8_STRING_TYPE
        COSEM_INTEGER_TYPE
        COSEM_LONG_TYPE
        COSEM_UNSIGNED_TYPE
        COSEM_LONG_UNSIGNED_TYPE
        COSEM_LONG64_TYPE
        COSEM_LONG64_UNSIGNED_TYPE
        COSEM_FLOAT32_TYPE
        COSEM_FLOAT64_TYPE
    COSEM_END_CHOICE
COSEM_END_SCHEMA
// enum
enum StructureTest1Enum
{
    Unit1,
    Unit2,
    Unit3
};
COSEM_BEGIN_SCHEMA(Enum_Schema_Test1)
    COSEM_ENUM_TYPE({ StructureTest1Enum::Unit1, StructureTest1Enum::Unit2, StructureTest1Enum::Unit3 })
COSEM_END_SCHEMA
// structure
// scal-unit-type
COSEM_BEGIN_SCHEMA(Structure_Schema_Test1)
    COSEM_BEGIN_STRUCTURE
        COSEM_INTEGER_TYPE
        COSEM_ENUM_TYPE({StructureTest1Enum::Unit1, StructureTest1Enum::Unit2, StructureTest1Enum::Unit3})
    COSEM_END_STRUCTURE
COSEM_END_SCHEMA
// double-long-unsigned
COSEM_BEGIN_SCHEMA(Double_Long_Unsigned_Schema_Test1)
    COSEM_DOUBLE_LONG_UNSIGNED_TYPE
COSEM_END_SCHEMA
// long-unsigned
COSEM_BEGIN_SCHEMA(Long_Unsigned_Schema_Test1)
    COSEM_LONG_UNSIGNED_TYPE
COSEM_END_SCHEMA
// array
COSEM_BEGIN_SCHEMA(Array_Schema_Test1)
    COSEM_BEGIN_ARRAY
        COSEM_UNSIGNED_TYPE
    COSEM_END_ARRAY
COSEM_END_SCHEMA
// array of simple structures
COSEM_BEGIN_SCHEMA(Array_Schema_Test2)
    COSEM_BEGIN_ARRAY
        COSEM_BEGIN_STRUCTURE
            COSEM_LONG_UNSIGNED_TYPE
            COSEM_OCTET_STRING_TYPE
        COSEM_END_STRUCTURE
    COSEM_END_ARRAY
COSEM_END_SCHEMA
// array of structures
//     octet-string
//     array of unsigned
COSEM_BEGIN_SCHEMA(Array_Schema_Test3)
    COSEM_BEGIN_ARRAY
        COSEM_BEGIN_STRUCTURE
            COSEM_OCTET_STRING_TYPE
            COSEM_BEGIN_ARRAY
                COSEM_UNSIGNED_TYPE
            COSEM_END_ARRAY
        COSEM_END_STRUCTURE
    COSEM_END_ARRAY
COSEM_END_SCHEMA
// compact-array
// structure
//     unsigned
//     CHOICE
COSEM_BEGIN_SCHEMA(Structure_Schema_Test2)
    COSEM_BEGIN_STRUCTURE
        COSEM_UNSIGNED_TYPE
        COSEM_BEGIN_CHOICE
            COSEM_NULL_DATA_TYPE
            COSEM_BEGIN_ARRAY
                COSEM_INTEGER_TYPE
            COSEM_END_ARRAY
        COSEM_END_CHOICE
    COSEM_END_STRUCTURE
COSEM_END_SCHEMA

TEST(COSEMData, Constructors) 
{
    COSEMType Empty;
    ASSERT_TRUE(Empty.IsEmpty());
    ASSERT_FALSE(Empty.Append(42));
    
    COSEMType Test1(Octet_String_Schema_Test);
    const DLMSVector TEST1({ 1, 2, 3, 4 });
    ASSERT_TRUE(Test1.Append(TEST1));
}

TEST(COSEMData, CHOICE) 
{
    COSEMType Test1(Choice_Schema_Test1);
    ASSERT_TRUE(Test1.SelectChoice(COSEMDataType::NULL_DATA));
    ASSERT_TRUE(Test1.Append());
    
    Test1.Clear();
    ASSERT_TRUE(Test1.SelectChoice(COSEMDataType::BIT_STRING));
    COSEMBitString  BitString1(8, 42);
    ASSERT_TRUE(Test1.Append(BitString1));

    Test1.Clear();
    ASSERT_TRUE(Test1.SelectChoice(COSEMDataType::DOUBLE_LONG));
    ASSERT_TRUE(Test1.Append(-23872872));

    Test1.Clear();
    ASSERT_TRUE(Test1.SelectChoice(COSEMDataType::DOUBLE_LONG_UNSIGNED));
    ASSERT_TRUE(Test1.Append(0x539343));

    Test1.Clear();
    ASSERT_TRUE(Test1.SelectChoice(COSEMDataType::OCTET_STRING));
    const DLMSVector TEST1({ 1, 2, 3, 4 });
    ASSERT_TRUE(Test1.Append(TEST1));

    Test1.Clear();
    ASSERT_TRUE(Test1.SelectChoice(COSEMDataType::VISIBLE_STRING));
    const std::string TEST2("TEST");
    ASSERT_TRUE(Test1.Append(TEST2));

    Test1.Clear();
    ASSERT_TRUE(Test1.SelectChoice(COSEMDataType::INTEGER));
    ASSERT_TRUE(Test1.Append(-42));

    Test1.Clear();
    ASSERT_TRUE(Test1.SelectChoice(COSEMDataType::LONG));
    ASSERT_TRUE(Test1.Append(-4243334));

    Test1.Clear();
    ASSERT_TRUE(Test1.SelectChoice(COSEMDataType::UNSIGNED));
    ASSERT_TRUE(Test1.Append(0x5423));

    Test1.Clear();
    ASSERT_TRUE(Test1.SelectChoice(COSEMDataType::LONG_UNSIGNED));
    ASSERT_TRUE(Test1.Append(0x355423));

    Test1.Clear();
    ASSERT_TRUE(Test1.SelectChoice(COSEMDataType::LONG64));
    ASSERT_TRUE(Test1.Append(-38373878373383));

    Test1.Clear();
    ASSERT_TRUE(Test1.SelectChoice(COSEMDataType::LONG64_UNSIGNED));
    ASSERT_TRUE(Test1.Append(0x49484947434));

//    Test1.Clear();
//    ASSERT_TRUE(Test1.SelectChoice(COSEMDataType::FLOAT32));
//    ASSERT_TRUE(Test1.Append(-4.56843f));
//
//    Test1.Clear();
//    ASSERT_TRUE(Test1.SelectChoice(COSEMDataType::FLOAT64));
//    ASSERT_TRUE(Test1.Append(-443243243234.613234343));
    
}

