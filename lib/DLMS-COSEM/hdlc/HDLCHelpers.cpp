#include "hdlc/HDLCHelpers.h"

namespace EPRI
{
    #define IS_BIG_ENDIAN (*(uint16_t *)"\0\xff" < 0x100)

    uint16_t Get16BigEndianHelper(const uint8_t * pBuffer)
    {
        if (IS_BIG_ENDIAN)
        {
            return *((uint16_t *) pBuffer);
        }
        else
        {
            union
            {
                uint16_t u16;
                uint8_t  u8[sizeof(uint16_t)];
            } Converter;
            Converter.u8[0] = pBuffer[1];
            Converter.u8[1] = pBuffer[0];
            return Converter.u16;
        }
    }

    uint16_t Get32BigEndianHelper(const uint8_t * pBuffer)
    {
        if (IS_BIG_ENDIAN)
        {
            return *((uint32_t *) pBuffer);
        }
        else
        {
            union
            {
                uint32_t u32;
                uint8_t  u8[sizeof(uint32_t)];
            } Converter;
            Converter.u8[0] = pBuffer[3];
            Converter.u8[1] = pBuffer[2];
            Converter.u8[2] = pBuffer[1];
            Converter.u8[3] = pBuffer[0];
            return Converter.u32;
        }
    }

}