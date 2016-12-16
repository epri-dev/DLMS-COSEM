#pragma once

#include <cstdint>
#include <cstddef>

class CircularBuffer
{
public:
    enum Result
    {
        OK = 0,
        INVALID_PARAM,
        EMPTY,
        FULL,
        BUFF_EOF
    };
    CircularBuffer() = delete;
    CircularBuffer(uint8_t * pBuffer, size_t BufferSize);
    ~CircularBuffer();
    
    Result Put(uint8_t * pBytesToPut,
        size_t NumberOfBytesToPut,
        size_t * pNumberOfBytesActual);
    Result Get(uint8_t * pBytesToGet,
        size_t NumberOfBytesToGet,
        size_t * pNumberOfBytesActual);
    Result Peek(size_t Index,
        uint8_t * pBytesToGet,
        size_t NumberOfBytesToGet,
        size_t * pNumberOfBytesActual);
    Result Count(size_t * pCount);
    bool CanFit(size_t RequestedCount);
    Result Clear();
    
private:
    size_t    m_Head;
    size_t    m_Tail;
    uint8_t * m_pBuffer;
    size_t    m_BufferSize;
    
};

