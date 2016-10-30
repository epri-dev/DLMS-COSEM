#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/DLMSVector.cpp"

using namespace EPRI;

TEST(DLMSVector, InitialSettings) 
{
    uint8_t u8;
    
    DLMSVariant Variant1;
    DLMSVector Vector1;
    ASSERT_EQ(0, Vector1.Size());
    ASSERT_EQ(0, Vector1.GetReadPosition());
    ASSERT_FALSE(Vector1.SetReadPosition(1));

    ASSERT_FALSE(Vector1.GetUInt8(&Variant1));
    ASSERT_FALSE(Vector1.GetUInt16(&Variant1));
    ASSERT_FALSE(Vector1.GetUInt32(&Variant1));
    ASSERT_FALSE(Vector1.GetUInt64(&Variant1));
    ASSERT_FALSE(Vector1.GetInt8(&Variant1));
    ASSERT_FALSE(Vector1.GetInt16(&Variant1));
    ASSERT_FALSE(Vector1.GetInt32(&Variant1));
    ASSERT_FALSE(Vector1.GetInt64(&Variant1));
    ASSERT_FALSE(Vector1.GetFloat(&Variant1));
    ASSERT_FALSE(Vector1.GetDouble(&Variant1));
    ASSERT_FALSE(Vector1.Get(&u8, 1));
    ASSERT_EQ(0, Vector1.GetBytes().size());
    ASSERT_EQ(0, Vector1.ToString().length());
}

TEST(DLMSVector, AppendBigEndian) 
{
//    uint8_t u8;
//    uint16_t u16;
//    uint32_t u32;
//    uint64_t u64;
//    int8_t  i8;
//    int16_t i16;
//    int32_t i32;
//    int64_t i64;
//    float   f;
//    double  d;
    
    DLMSVariant Variant1;
    DLMSVector Vector1;
    
    Vector1.AppendUInt8(1);
    ASSERT_EQ(1, Vector1.Size());
    Vector1.AppendUInt16(0x1234);
    ASSERT_EQ(3, Vector1.Size());  
    Vector1.AppendUInt32(0x56789ABC);
    ASSERT_EQ(7, Vector1.Size());  
    Vector1.AppendUInt64(0xDEF0123456789ABC);
    ASSERT_EQ(15, Vector1.Size());  
    Vector1.AppendInt8(-1);
    ASSERT_EQ(16, Vector1.Size());  
    Vector1.AppendInt16(-1000);
    ASSERT_EQ(18, Vector1.Size());  
    Vector1.AppendInt32(-20000000);
    ASSERT_EQ(22, Vector1.Size());  
    Vector1.AppendInt64(-400000000000);
    ASSERT_EQ(30, Vector1.Size());  
    Vector1.AppendFloat(-42.1234);
    ASSERT_EQ(34, Vector1.Size());  
    Vector1.AppendDouble(-123456789.0123456789);
    ASSERT_EQ(42, Vector1.Size());  
    
    const uint8_t BYTES[] = { 0x87, 0x62, 0x10 };
    Vector1.Append(BYTES, sizeof(BYTES));
    ASSERT_EQ(45, Vector1.Size());  
   
    ASSERT_EQ(0, Vector1.GetReadPosition());
    ASSERT_TRUE(Vector1.GetUInt8(&Variant1));
    ASSERT_EQ(1, Variant1.get<uint8_t>());
    ASSERT_TRUE(Vector1.GetUInt16(&Variant1));
    ASSERT_EQ(0x1234, Variant1.get<uint16_t>());
    ASSERT_TRUE(Vector1.GetUInt32(&Variant1));
    ASSERT_EQ(0x56789ABC, Variant1.get<uint32_t>());
    ASSERT_TRUE(Vector1.GetUInt64(&Variant1));
    ASSERT_EQ(0xDEF0123456789ABC, Variant1.get<uint64_t>());
    ASSERT_TRUE(Vector1.GetInt8(&Variant1));
    ASSERT_EQ(-1, Variant1.get<int8_t>());
    ASSERT_TRUE(Vector1.GetInt16(&Variant1));
    ASSERT_EQ(-1000, Variant1.get<int16_t>());
    ASSERT_TRUE(Vector1.GetInt32(&Variant1));
    ASSERT_EQ(-20000000, Variant1.get<int32_t>());
    ASSERT_TRUE(Vector1.GetInt64(&Variant1));
    ASSERT_EQ(-400000000000, Variant1.get<int64_t>());
    ASSERT_TRUE(Vector1.GetFloat(&Variant1));
    ASSERT_FLOAT_EQ(-42.1234, Variant1.get<float>());
    ASSERT_TRUE(Vector1.GetDouble(&Variant1));
    ASSERT_DOUBLE_EQ(-123456789.0123456789, Variant1.get<double>());
    
    uint8_t GETBYTES[3];
    ASSERT_TRUE(Vector1.Get(GETBYTES, sizeof(GETBYTES)));
    ASSERT_EQ(0, std::memcmp(GETBYTES, BYTES, sizeof(BYTES)));

}

TEST(DLMSVector, AppendLittleEndian) 
{
//    uint8_t u8;
//    uint16_t u16;
//    uint32_t u32;
//    uint64_t u64;
//    int8_t  i8;
//    int16_t i16;
//    int32_t i32;
//    int64_t i64;
//    float   f;
//    double  d;
    
    DLMSVariant Variant1;
    DLMSVector Vector1;
    
    ASSERT_EQ(0, Vector1.AppendUInt8(1));
    ASSERT_EQ(1, Vector1.Size());
    ASSERT_EQ(1, Vector1.AppendUInt16(0x1234, false));
    ASSERT_EQ(3, Vector1.Size());  
    ASSERT_EQ(3, Vector1.AppendUInt32(0x56789ABC, false));
    ASSERT_EQ(7, Vector1.Size());  
    ASSERT_EQ(7, Vector1.AppendUInt64(0xDEF0123456789ABC, false));
    ASSERT_EQ(15, Vector1.Size());  
    ASSERT_EQ(15, Vector1.AppendInt8(-1));
    ASSERT_EQ(16, Vector1.Size());  
    ASSERT_EQ(16, Vector1.AppendInt16(-1000, false));
    ASSERT_EQ(18, Vector1.Size());  
    ASSERT_EQ(18, Vector1.AppendInt32(-20000000, false));
    ASSERT_EQ(22, Vector1.Size());  
    ASSERT_EQ(22, Vector1.AppendInt64(-400000000000, false));
    ASSERT_EQ(30, Vector1.Size());  
    ASSERT_EQ(30, Vector1.AppendFloat(-42.1234));
    ASSERT_EQ(34, Vector1.Size());  
    ASSERT_EQ(34, Vector1.AppendDouble(-123456789.0123456789));
    ASSERT_EQ(42, Vector1.Size());  
    
    const uint8_t BYTES[] = { 0x87, 0x62, 0x10 };
    ASSERT_EQ(42, Vector1.Append(BYTES, sizeof(BYTES)));
    ASSERT_EQ(45, Vector1.Size());  
   
    ASSERT_EQ(0, Vector1.GetReadPosition());
    ASSERT_TRUE(Vector1.GetUInt8(&Variant1));
    ASSERT_EQ(1, Variant1.get<uint8_t>());
    ASSERT_TRUE(Vector1.GetUInt16(&Variant1, false));
    ASSERT_EQ(0x1234, Variant1.get<uint16_t>());
    ASSERT_TRUE(Vector1.GetUInt32(&Variant1, false));
    ASSERT_EQ(0x56789ABC, Variant1.get<uint32_t>());
    ASSERT_TRUE(Vector1.GetUInt64(&Variant1, false));
    ASSERT_EQ(0xDEF0123456789ABC, Variant1.get<uint64_t>());
    ASSERT_TRUE(Vector1.GetInt8(&Variant1));
    ASSERT_EQ(-1, Variant1.get<int8_t>());
    ASSERT_TRUE(Vector1.GetInt16(&Variant1, false));
    ASSERT_EQ(-1000, Variant1.get<int16_t>());
    ASSERT_TRUE(Vector1.GetInt32(&Variant1, false));
    ASSERT_EQ(-20000000, Variant1.get<int32_t>());
    ASSERT_TRUE(Vector1.GetInt64(&Variant1, false));
    ASSERT_EQ(-400000000000, Variant1.get<int64_t>());
    ASSERT_TRUE(Vector1.GetFloat(&Variant1));
    ASSERT_FLOAT_EQ(-42.1234, Variant1.get<float>());
    ASSERT_TRUE(Vector1.GetDouble(&Variant1));
    ASSERT_DOUBLE_EQ(-123456789.0123456789, Variant1.get<double>());
    
    uint8_t GETBYTES[3];
    ASSERT_TRUE(Vector1.Get(GETBYTES, sizeof(GETBYTES)));
    ASSERT_EQ(0, std::memcmp(GETBYTES, BYTES, sizeof(BYTES)));

}

TEST(DLMSVector, AppendVariant) 
{
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    int8_t  i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    float   f;
    double  d;
    
    DLMSVariant Variant = 1;
    DLMSVector Vector1;
    
    ASSERT_EQ(0, Vector1.Append(Variant));
    Variant = 256;
    ASSERT_EQ(1, Vector1.Append(Variant));
    Variant = 65536;
    ASSERT_EQ(3, Vector1.Append(Variant));
    Variant = 0x1FFFFFFFF;
    ASSERT_EQ(7, Vector1.Append(Variant));
   
}


