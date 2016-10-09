#pragma once

#include <deque>
#include <vector>
#include <execinfo.h>

#include "ERROR_TYPE.h"
#include "ISerial.h"

class DummySerial : public EPRI::ISerial
{
public:
    DummySerial()
    {
    }
    //
    // ISerial
    //
    EPRI::ERROR_TYPE Open(SerialPort Port)
    {
        return EPRI::SUCCESSFUL;
    }
    Options GetOptions()
    {
        return Options();
    }
    EPRI::ERROR_TYPE SetOptions(const Options& /*Opt*/)
    {
        return EPRI::SUCCESSFUL;
    }
    EPRI::ERROR_TYPE Write(const uint8_t * pBuffer, size_t Bytes)
    {
        int Who = WhoCalled();
        while (Bytes--)
        {
            m_SerialStream[Who].push_back(*pBuffer++) ;
        }
        return EPRI::SUCCESSFUL;
    }
    EPRI::ERROR_TYPE Read(uint8_t * pBuffer, size_t MaxBytes, uint32_t TimeOutInMS = 0, size_t * pActualBytes = nullptr)
    {
        int Who = WhoCalled() ^ 1;
        if (m_SerialStream[Who].size())
        {
            while (MaxBytes--)
            {
                *pBuffer++ = m_SerialStream[Who].front();
                m_SerialStream[Who].pop_front();
            }
            return EPRI::SUCCESSFUL;
        }
        return ~EPRI::SUCCESSFUL;
    }
    EPRI::ERROR_TYPE Close()
    {
        return EPRI::SUCCESSFUL;
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
    std::deque<uint8_t>  m_SerialStream[2];

};

