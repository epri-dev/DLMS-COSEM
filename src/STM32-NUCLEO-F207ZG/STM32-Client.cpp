#include <iostream>
#include <cstdio>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <iomanip>

#include "STM32-Client.h"
#include "STM32BaseLibrary.h"

#include "HDLCLLC.h"
#include "COSEM.h"
#include "serialwrapper/SerialWrapper.h"

using namespace std;
using namespace EPRI;

void RunClient()
{
    STM32BaseLibrary     bl;
    ISerial *		     pSerial = bl.GetCore()->GetSerial();
    int                  opt;
    bool                 StartWithIEC = false;
    bool                 IsSerialWrapper = true;
    const char *         pCOMPortName = "COM1";
    const char *         pSourceAddress = "1";
    const char *         pDestinationAddress = "1";
    const char *         pPassword = "33333333";
    COSEM::SecurityLevel Security = COSEM::SECURITY_LOW_LEVEL;
    
    
    if (nullptr != pCOMPortName &&
        nullptr != pSourceAddress &&
        nullptr != pDestinationAddress)
    {
        enum States
        {
            ST_OPEN,
            ST_OPEN_WAIT,
            ST_OPENED,
            ST_GET_WAIT,
            ST_GET_DONE
        }                           CurrentState = ST_OPEN;

        uint8_t                     SourceAddress = uint8_t(strtol(pSourceAddress, nullptr, 16));
        uint8_t                     DestinationAddress = uint8_t(strtol(pDestinationAddress, nullptr, 16));
        ISerial::Options::BaudRate  BR = STM32Serial::Options::BAUD_9600;
        COSEMClient                 APClient;
        HDLCClientLLC               DLClient(HDLCAddress(SourceAddress), 
            pSerial,
            HDLCOptions({ StartWithIEC, 3, 500 }));
        SerialWrapper               DLWrapper(pSerial, SerialWrapper::WrapperPorts(SourceAddress, DestinationAddress));
        COSEMClient::CallbackFunction 
                                    OpenConfirm = [&](const BaseCallbackParameter& _) -> bool
        {
            std::cout << "Association Established\n";
            return true;
        };
        COSEMClient::CallbackFunction 
                                    GetConfirm = [&](const BaseCallbackParameter& Param) -> bool
        {
            std::cout << "Data Retrieved\n";
            std::cout << dynamic_cast<const APPGetConfirmOrResponse&>(Param).m_Data.ToString() << std::endl;
            CurrentState = ST_GET_DONE;
            return true;
        };
        
        std::cout << "Operating as a Client (0x" << hex << uint16_t(SourceAddress) << ")... Opening " << pCOMPortName << "\n";
        if (pSerial->Open(pCOMPortName) != SUCCESS ||
            pSerial->SetOptions(STM32Serial::Options(BR)) != SUCCESS)
        {
            std::cerr << "Error opening port " << pCOMPortName << "\n";
            exit(-1);
        }

        std::cout << "Connecting to Server (0x" << std::hex << uint16_t(DestinationAddress) << ")\n";
        if (IsSerialWrapper)
        {
            std::string Command;
            bool bProcessed = false;
            
            APClient.RegisterTransport(&DLWrapper);
            APClient.RegisterOpenConfirm(OpenConfirm);
            APClient.RegisterGetConfirm(GetConfirm);
            
            bool NeedToProcess = true;
            while (true)
            {
                if (NeedToProcess)
                {
                    DLWrapper.Process();
                }
                switch (CurrentState)
                {
                case ST_OPEN:
                    APClient.OpenRequest(APPOpenRequestOrIndication(Security, pPassword == nullptr ? "" : pPassword));
                    CurrentState = ST_OPEN_WAIT;
                    break;
                case ST_OPEN_WAIT:
                    if (APClient.IsOpen())
                        CurrentState = ST_OPENED;
                    break;
                case ST_OPENED:
                    APClient.GetRequest(APPGetRequestOrIndication(COSEMAttributeDescriptor(8, { 0, 0, 1, 0, 0, 255 }, 2)));
                    CurrentState = ST_GET_WAIT;
                    break;
                case ST_GET_WAIT:
                    break;
                case ST_GET_DONE:
                    NeedToProcess = false;
                    break;
                }
            }
        }    
    }
    
}
