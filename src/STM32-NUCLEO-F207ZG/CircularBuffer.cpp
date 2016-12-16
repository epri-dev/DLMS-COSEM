#include <FreeRTOS.h>
#include <task.h>

#include "CircularBuffer.h"

CircularBuffer::CircularBuffer(uint8_t * pBuffer, size_t BufferSize)
{
    m_Head = 0;
    m_Tail = 0;
    m_pBuffer = pBuffer;
    m_BufferSize = BufferSize;
}

CircularBuffer::~CircularBuffer()
{
}

CircularBuffer::Result CircularBuffer::Put(uint8_t * pBytesToPut,
    size_t NumberOfBytesToPut,
    size_t * pNumberOfBytesActual)
{
    Result            RetVal = OK;
    *pNumberOfBytesActual = 0;
    while (*pNumberOfBytesActual < NumberOfBytesToPut)
    {
        
        m_pBuffer[m_Head] = pBytesToPut[(*pNumberOfBytesActual)++];
        UBaseType_t IntStatus = taskENTER_CRITICAL_FROM_ISR();
        if (++m_Head >= m_BufferSize)
            m_Head = 0;
        if (m_Head == m_Tail)
        {
            taskEXIT_CRITICAL_FROM_ISR(IntStatus);
            RetVal = FULL;
            break;
        }
        taskEXIT_CRITICAL_FROM_ISR(IntStatus);

    }

    return RetVal;
}

CircularBuffer::Result CircularBuffer::Get(uint8_t * pBytesToGet,
    size_t NumberOfBytesToGet,
    size_t * pNumberOfBytesActual)
{
    Result            RetVal = OK;

    *pNumberOfBytesActual = 0;
    while ((*pNumberOfBytesActual < NumberOfBytesToGet) &&
            (m_Head != m_Tail))
    {
        pBytesToGet[(*pNumberOfBytesActual)++] = m_pBuffer[m_Tail];
        UBaseType_t IntStatus = taskENTER_CRITICAL_FROM_ISR();
        if (++m_Tail >= m_BufferSize)
            m_Tail = 0;
        if (m_Head == m_Tail &&
             (*pNumberOfBytesActual < NumberOfBytesToGet))
        {
            taskEXIT_CRITICAL_FROM_ISR(IntStatus);
            RetVal = EMPTY;
            break;
        }
        taskEXIT_CRITICAL_FROM_ISR(IntStatus);
    } 

    return RetVal;

} 

CircularBuffer::Result CircularBuffer::Peek(size_t Index,
    uint8_t * pBytesToGet,
    size_t NumberOfBytesToGet,
    size_t * pNumberOfBytesActual)
{
    size_t      CurrentCount;
    size_t      CurrentTail;
    Result      RetVal = Count(&CurrentCount); 

    if (Index > CurrentCount)
    {
        RetVal = BUFF_EOF;
    }
    else if (OK == RetVal)
    {
        CurrentTail = m_Tail;
        *pNumberOfBytesActual = 0;
        while ((*pNumberOfBytesActual < NumberOfBytesToGet) &&
                (m_Head != CurrentTail))
        {
            pBytesToGet[(*pNumberOfBytesActual)++] = m_pBuffer[(CurrentTail + Index) % m_BufferSize];
            if (++CurrentTail >= m_BufferSize)
                CurrentTail = 0;
            if (m_Head == CurrentTail &&
                 (*pNumberOfBytesActual < NumberOfBytesToGet))
            {
                RetVal = EMPTY;
                break;
            }
        }
    }
    return RetVal;

}

CircularBuffer::Result CircularBuffer::Count(size_t * pCount)
{
    UBaseType_t IntStatus = taskENTER_CRITICAL_FROM_ISR();
    if (m_Head >= m_Tail)
        *pCount = m_Head - m_Tail;
    else if (m_Tail > m_Head)
        *pCount = m_BufferSize - m_Tail + m_Head;
    taskEXIT_CRITICAL_FROM_ISR(IntStatus);

    return OK;

} 

bool CircularBuffer::CanFit(size_t RequestedCount)
{
    size_t  CurrentCount;
    if (OK == (Count(&CurrentCount)))
        return ((CurrentCount + RequestedCount) <= m_BufferSize);
    return false;

} 

CircularBuffer::Result CircularBuffer::Clear()
{
    UBaseType_t IntStatus = taskENTER_CRITICAL_FROM_ISR();
    m_Head = m_Tail = 0;
    taskEXIT_CRITICAL_FROM_ISR(IntStatus);

    return OK;

}