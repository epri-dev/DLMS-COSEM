#include <gtest/gtest.h>

#include "../../lib/DLMS-COSEM/DLMSVector.cpp"

using namespace EPRI;

TEST(DLMSVector, InitialSettings) 
{
    uint8_t u8;
    std::string String;
    
    DLMSVariant Variant1;
    DLMSVector Vector1;
    ASSERT_TRUE(Vector1.IsAtEnd());
    ASSERT_EQ(0, Vector1.Size());
    ASSERT_EQ(0, Vector1.GetReadPosition());
    ASSERT_FALSE(Vector1.SetReadPosition(1));
    ASSERT_FALSE(Vector1.Skip(1));

    ASSERT_FALSE(Vector1.Get<uint8_t>(&Variant1));
    ASSERT_FALSE(Vector1.Get<uint16_t>(&Variant1));
    ASSERT_FALSE(Vector1.Get<uint32_t>(&Variant1));
    ASSERT_FALSE(Vector1.Get<uint64_t>(&Variant1));
    ASSERT_FALSE(Vector1.Get<int8_t>(&Variant1));
    ASSERT_FALSE(Vector1.Get<int16_t>(&Variant1));
    ASSERT_FALSE(Vector1.Get<int32_t>(&Variant1));
    ASSERT_FALSE(Vector1.Get<int64_t>(&Variant1));
    ASSERT_FALSE(Vector1.Get<float>(&Variant1));
    ASSERT_FALSE(Vector1.Get<double>(&Variant1));
    ASSERT_FALSE(Vector1.GetBuffer(&u8, 1));
    ASSERT_FALSE(Vector1.Get(&String, 1));
    ASSERT_EQ(0, Vector1.GetBytes().size());
    ASSERT_EQ(0, Vector1.ToString().length());
}

TEST(DLMSVector, AppendGeneral)
{
    DLMSVector Vector1;
    
    ASSERT_EQ(0, Vector1.Size());
    ASSERT_EQ(0, Vector1.AppendExtra(10));
    ASSERT_EQ(10, Vector1.Size());
    Vector1[5] = 0x42;
    ASSERT_EQ(0x42, Vector1[5]);
    const std::vector<uint8_t> VALIDATE1 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x42, 0x00, 0x00, 0x00, 0x00 };
    ASSERT_EQ(VALIDATE1, Vector1.GetBytes());
    ASSERT_TRUE(Vector1.Zero());
    const std::vector<uint8_t> VALIDATE2 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };    
    ASSERT_EQ(VALIDATE2, Vector1.GetBytes());
    Vector1.Clear();
    ASSERT_EQ(0, Vector1.Size());

}

TEST(DLMSVector, SkipsAndReads)
{
    DLMSVector Vector1({ 1, 2, 3, 4, 5, 6, 7 });
    ASSERT_EQ(7, Vector1.Size());
    ASSERT_FALSE(Vector1.IsAtEnd());
    ASSERT_TRUE(Vector1.Skip(3));
    ASSERT_EQ(4, Vector1.PeekByte());
    ASSERT_FALSE(Vector1.Skip(10));
    ASSERT_EQ(3, Vector1.GetReadPosition());
    ASSERT_EQ(4, Vector1.Get<uint8_t>());
    ASSERT_EQ(4, Vector1.GetReadPosition());
    uint8_t BUFFER[10] = { };
    const uint8_t COMPAREBUFFER[] = { 5, 6, 7 };
    ASSERT_TRUE(Vector1.PeekBuffer(BUFFER, 3));
    ASSERT_EQ(0, std::memcmp(BUFFER, COMPAREBUFFER, 3));
    ASSERT_EQ(4, Vector1.GetReadPosition());
    
}

TEST(DLMSVector, Operators)
{
    DLMSVector Vector1({ 1, 2, 3, 4 });
    DLMSVector Vector2(std::vector<uint8_t>({ 1, 2, 3, 4 }));   
    DLMSVector Vector3;
    ASSERT_EQ(Vector1, Vector2);
    
    Vector3 = Vector1;
    ASSERT_EQ(Vector1, Vector3);
}

TEST(DLMSVector, AppendBigEndian) 
{
    DLMSVariant Variant1;
    DLMSVector Vector1;
    
    Vector1.Append<uint8_t>(1);
    ASSERT_EQ(1, Vector1.Size());
    Vector1.Append<uint16_t>(0x1234);
    ASSERT_EQ(3, Vector1.Size());  
    Vector1.Append<uint32_t>(0x56789ABC);
    ASSERT_EQ(7, Vector1.Size());  
    Vector1.Append<uint64_t>(0xDEF0123456789ABC);
    ASSERT_EQ(15, Vector1.Size());  
    Vector1.Append<int8_t>(-1);
    ASSERT_EQ(16, Vector1.Size());  
    Vector1.Append<int16_t>(-1000);
    ASSERT_EQ(18, Vector1.Size());  
    Vector1.Append<int32_t>(-20000000);
    ASSERT_EQ(22, Vector1.Size());  
    Vector1.Append<int64_t>(-400000000000);
    ASSERT_EQ(30, Vector1.Size());  
    Vector1.AppendFloat(-42.1234);
    ASSERT_EQ(34, Vector1.Size());  
    Vector1.AppendDouble(-123456789.0123456789);
    ASSERT_EQ(42, Vector1.Size());  
    
    const uint8_t BYTES[] = { 0x87, 0x62, 0x10 };
    Vector1.AppendBuffer(BYTES, sizeof(BYTES));
    ASSERT_EQ(45, Vector1.Size());  
   
    ASSERT_EQ(0, Vector1.GetReadPosition());
    ASSERT_TRUE(Vector1.Get<uint8_t>(&Variant1));
    ASSERT_EQ(1, Variant1.get<uint8_t>());
    ASSERT_TRUE(Vector1.Get<uint16_t>(&Variant1));
    ASSERT_EQ(0x1234, Variant1.get<uint16_t>());
    ASSERT_TRUE(Vector1.Get<uint32_t>(&Variant1));
    ASSERT_EQ(0x56789ABC, Variant1.get<uint32_t>());
    ASSERT_TRUE(Vector1.Get<uint64_t>(&Variant1));
    ASSERT_EQ(0xDEF0123456789ABC, Variant1.get<uint64_t>());
    ASSERT_TRUE(Vector1.Get<int8_t>(&Variant1));
    ASSERT_EQ(-1, Variant1.get<int8_t>());
    ASSERT_TRUE(Vector1.Get<int16_t>(&Variant1));
    ASSERT_EQ(-1000, Variant1.get<int16_t>());
    ASSERT_TRUE(Vector1.Get<int32_t>(&Variant1));
    ASSERT_EQ(-20000000, Variant1.get<int32_t>());
    ASSERT_TRUE(Vector1.Get<int64_t>(&Variant1));
    ASSERT_EQ(-400000000000, Variant1.get<int64_t>());
    ASSERT_TRUE(Vector1.Get<float>(&Variant1));
    ASSERT_FLOAT_EQ(-42.1234, Variant1.get<float>());
    ASSERT_TRUE(Vector1.Get<double>(&Variant1));
    ASSERT_DOUBLE_EQ(-123456789.0123456789, Variant1.get<double>());
    
    uint8_t GETBYTES[3];
    ASSERT_TRUE(Vector1.GetBuffer(GETBYTES, sizeof(GETBYTES)));
    ASSERT_EQ(0, std::memcmp(GETBYTES, BYTES, sizeof(BYTES)));

}

TEST(DLMSVector, AppendLittleEndian) 
{
    
    DLMSVariant Variant1;
    DLMSVector Vector1;
    
    ASSERT_EQ(0, Vector1.Append<uint8_t>(1));
    ASSERT_EQ(1, Vector1.Size());
    ASSERT_EQ(1, Vector1.Append<uint16_t>(0x1234, false));
    ASSERT_EQ(3, Vector1.Size());  
    ASSERT_EQ(3, Vector1.Append<uint32_t>(0x56789ABC, false));
    ASSERT_EQ(7, Vector1.Size());  
    ASSERT_EQ(7, Vector1.Append<uint64_t>(0xDEF0123456789ABC, false));
    ASSERT_EQ(15, Vector1.Size());  
    ASSERT_EQ(15, Vector1.Append<int8_t>(-1));
    ASSERT_EQ(16, Vector1.Size());  
    ASSERT_EQ(16, Vector1.Append<int16_t>(-1000, false));
    ASSERT_EQ(18, Vector1.Size());  
    ASSERT_EQ(18, Vector1.Append<int32_t>(-20000000, false));
    ASSERT_EQ(22, Vector1.Size());  
    ASSERT_EQ(22, Vector1.Append<int64_t>(-400000000000, false));
    ASSERT_EQ(30, Vector1.Size());  
    ASSERT_EQ(30, Vector1.AppendFloat(-42.1234));
    ASSERT_EQ(34, Vector1.Size());  
    ASSERT_EQ(34, Vector1.AppendDouble(-123456789.0123456789));
    ASSERT_EQ(42, Vector1.Size());  
    
    const uint8_t BYTES[] = { 0x87, 0x62, 0x10 };
    ASSERT_EQ(42, Vector1.AppendBuffer(BYTES, sizeof(BYTES)));
    ASSERT_EQ(45, Vector1.Size());  
   
    ASSERT_EQ(0, Vector1.GetReadPosition());
    ASSERT_TRUE(Vector1.Get<uint8_t>(&Variant1));
    ASSERT_EQ(1, Variant1.get<uint8_t>());
    ASSERT_TRUE(Vector1.Get<uint16_t>(&Variant1, false));
    ASSERT_EQ(0x1234, Variant1.get<uint16_t>());
    ASSERT_TRUE(Vector1.Get<uint32_t>(&Variant1, false));
    ASSERT_EQ(0x56789ABC, Variant1.get<uint32_t>());
    ASSERT_TRUE(Vector1.Get<uint64_t>(&Variant1, false));
    ASSERT_EQ(0xDEF0123456789ABC, Variant1.get<uint64_t>());
    ASSERT_TRUE(Vector1.Get<int8_t>(&Variant1));
    ASSERT_EQ(-1, Variant1.get<int8_t>());
    ASSERT_TRUE(Vector1.Get<int16_t>(&Variant1, false));
    ASSERT_EQ(-1000, Variant1.get<int16_t>());
    ASSERT_TRUE(Vector1.Get<int32_t>(&Variant1, false));
    ASSERT_EQ(-20000000, Variant1.get<int32_t>());
    ASSERT_TRUE(Vector1.Get<int64_t>(&Variant1, false));
    ASSERT_EQ(-400000000000, Variant1.get<int64_t>());
    ASSERT_TRUE(Vector1.Get<float>(&Variant1));
    ASSERT_FLOAT_EQ(-42.1234, Variant1.get<float>());
    ASSERT_TRUE(Vector1.Get<double>(&Variant1));
    ASSERT_DOUBLE_EQ(-123456789.0123456789, Variant1.get<double>());
    
    uint8_t GETBYTES[3];
    ASSERT_TRUE(Vector1.GetBuffer(GETBYTES, sizeof(GETBYTES)));
    ASSERT_EQ(0, std::memcmp(GETBYTES, BYTES, sizeof(BYTES)));

}

TEST(DLMSVector, AppendVariant) 
{
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    int8_t  i8 = -1;
    int16_t i16 = -32000;
    int32_t i32 = -34000000;
    int64_t i64 = -6500000000;
    float   f = -38738.23;
    double  d = -498459849894894.4784784748487;
    
    DLMSVariant Variant = 1;
    DLMSVector Vector1;
    
    ASSERT_EQ(0, Vector1.Append(Variant));
    Variant = 256;
    ASSERT_EQ(1, Vector1.Append(Variant));
    Variant = 65536;
    ASSERT_EQ(3, Vector1.Append(Variant));
    Variant = 0x1FFFFFFFF;
    ASSERT_EQ(7, Vector1.Append(Variant));
    Variant = i8;
    ASSERT_EQ(13, Vector1.Append(Variant));
   
}

TEST(DLMSVector, AppendString)
{
    DLMSVector Vector1;
    std::string String;
    
    ASSERT_EQ(0, Vector1.Append(std::string("1234567")));
    ASSERT_EQ(7, Vector1.Append(std::string("890")));
    ASSERT_TRUE(Vector1.Get(&String, 7));
    ASSERT_EQ(String, "1234567");
    ASSERT_TRUE(Vector1.Get(&String, 3));
    ASSERT_EQ(String, "890");
    ASSERT_FALSE(Vector1.Get(&String, 3));

    Vector1.Clear();
    ASSERT_EQ(0, Vector1.Append(std::string("123")));
    ASSERT_EQ(3, Vector1.Append(std::string("456")));
    
    std::string String1;
    ASSERT_TRUE(Vector1.Get(&String1, 2, true));
    ASSERT_EQ(String1, "12");
    ASSERT_TRUE(Vector1.Get(&String1, 4, true));
    ASSERT_EQ(String1, "123456");
    
}


