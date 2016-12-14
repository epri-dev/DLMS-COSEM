#pragma once

#include <deque>
#include <vector>
#include <execinfo.h>

#include "ERROR_TYPE.h"
#include "ISerial.h"

using namespace EPRI;

class DummySerial : public EPRI::ISerialSocket
{
public:
    DummySerial()
    {
    }
    //
    // ISerialSocket
    //
    EPRI::ERROR_TYPE Open(const char * DestinationAddress = nullptr, int Port = DEFAULT_DLMS_PORT)
    {
        return EPRI::SUCCESSFUL;
    }
    ConnectCallbackFunction RegisterConnectHandler(ConnectCallbackFunction Callback)
    {
        ConnectCallbackFunction RetVal = m_Connect;
        m_Connect = Callback;
        return RetVal;
    }
    EPRI::ERROR_TYPE SetOptions(const EPRI::ISerial::Options& /*Opt*/)
    {
        return EPRI::SUCCESSFUL;
    }
    EPRI::ERROR_TYPE Write(const DLMSVector& Data, bool Asynchronous = false)
    {
        int Who = WhoCalled();
        for (int Index = 0; Index < Data.Size(); ++Index)
        {
            m_SerialStream[Who].push_back(Data[Index]);
        }
        return EPRI::SUCCESSFUL;
    }
    WriteCallbackFunction RegisterWriteHandler(WriteCallbackFunction Callback)
    {
        WriteCallbackFunction RetVal = m_Write;
        m_Write = Callback;
        return RetVal;
    }
    ERROR_TYPE Read(DLMSVector * pData,
        size_t ReadAtLeast = 0,
        uint32_t TimeOutInMS = 0,
        size_t * pActualBytes = nullptr)
    {
        int Who = WhoCalled() ^ 1;
        if (m_SerialStream[Who].size())
        {
            if (ReadAtLeast >= m_SerialStream[Who].size() || ReadAtLeast == 0)
            {
                ReadAtLeast = m_SerialStream[Who].size();
            }
            if (pData)
            {
                while (ReadAtLeast--)
                {
                    pData->Append<uint8_t>(m_SerialStream[Who].front());
                    m_SerialStream[Who].pop_front();
                }
            }
            else
            {
                while (ReadAtLeast--)
                {
                    m_AsyncBuffer[Who].Append<uint8_t>(m_SerialStream[Who].front());
                    m_SerialStream[Who].pop_front();
                }                    
            }
            return EPRI::SUCCESSFUL;
        }
        return ~EPRI::SUCCESSFUL;
    }
    bool AppendAsyncReadResult(DLMSVector * pData, size_t ReadAtLeast = 0)
    {
        int Who = WhoCalled() ^ 1;
        if (ReadAtLeast >= m_AsyncBuffer[Who].Size() || ReadAtLeast == 0)
        {
            ReadAtLeast = m_AsyncBuffer[Who].Size();
        }
        return pData->Append(m_AsyncBuffer[Who], 0, ReadAtLeast);
    }
    ReadCallbackFunction RegisterReadHandler(ReadCallbackFunction Callback)
    {
        ReadCallbackFunction RetVal = m_Read;
        m_Read = Callback;
        return RetVal;
    }
    EPRI::ERROR_TYPE Close()
    {
        return EPRI::SUCCESSFUL;
    }
    virtual CloseCallbackFunction RegisterCloseHandler(CloseCallbackFunction Callback)
    {
        CloseCallbackFunction RetVal = m_Close;
        m_Close = Callback;
        return RetVal;
    }
    EPRI::ERROR_TYPE Flush(FlushDirection Direction)
    {
        m_SerialStream[WhoCalled()].clear();
        return EPRI::SUCCESSFUL;
    }
    bool IsConnected()
    {
        return true;
    }
    
    bool IsWriteAsExpected(const std::vector<uint8_t>& Expected)
    {
        int Who = WhoCalled();
        if (m_SerialStream[Who].size() == Expected.size())
            return std::equal(m_SerialStream[Who].begin(), m_SerialStream[Who].end(), Expected.begin());
        return false;
    }
    
protected:
    int WhoCalled()
    {
        void * AddressList[64];
        int    AddressLength = backtrace(AddressList, sizeof(AddressList) / sizeof(void*));
        if (AddressLength)
        {
            char ** Symbols = backtrace_symbols(AddressList, AddressLength);
            for (int Index = 1; Index < AddressLength; Index++)
            {
                std::string Line(Symbols[Index]);
                if (Line.find("HDLCClient") != std::string::npos)
                    return 0;
                else if (Line.find("HDLCServer") != std::string::npos)
                    return 1;
            }
        }
        return 0;
    }
    
private:
    std::deque<uint8_t>     m_SerialStream[2];
    ConnectCallbackFunction m_Connect;
    WriteCallbackFunction   m_Write;
    ReadCallbackFunction    m_Read;
    CloseCallbackFunction   m_Close;
    DLMSVector              m_AsyncBuffer[2];

};

