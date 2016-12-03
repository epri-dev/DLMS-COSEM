#include <sstream>

#include "LinuxCOSEMServer.h"
#include "COSEMAddress.h"

namespace EPRI
{
    //
    // Data
    //
    LinuxData::LinuxData()
        : IDataObject({ 0, 0, 96, 1, {0, 9}, 255 })
    {
    }

    bool LinuxData::InternalGet(ICOSEMAttribute * pAttribute, 
        const Cosem_Attribute_Descriptor& Descriptor, 
        SelectiveAccess * pSelectiveAccess)
    {
        std::stringstream Output;
        Output << "LINUXDATA" << std::to_string(Descriptor.instance_id.GetValueGroup(EPRI::COSEMObjectInstanceID::VALUE_GROUP_E));
        pAttribute->SelectChoice(COSEMDataType::VISIBLE_STRING);
        pAttribute->Append(Output.str());
        return true;
    }
    //
    // Clock
    //
    LinuxClock::LinuxClock() :
        IClockObject({ 0, 0, 1, 0, 0, 255 })
    {
    }

    bool LinuxClock::InternalGet(ICOSEMAttribute * pAttribute, 
        const Cosem_Attribute_Descriptor& Descriptor, 
        SelectiveAccess * pSelectiveAccess)
    {
        switch (pAttribute->AttributeID)
        {
        case ATTR_TIME:
        case ATTR_TIME_ZONE:
        case ATTR_STATUS:
        case ATTR_DST_BEGIN:
        case ATTR_DST_END:
        case ATTR_DST_DEVIATION:
        case ATTR_DST_ENABLED:
        case ATTR_CLOCK_BASE:
        default:
            break;
        }
        return false;
    }
    //
    // Logical Device
    //
    LinuxManagementDevice::LinuxManagementDevice() :
        COSEMServer(ReservedAddresses::MANAGEMENT)
    {
        LOGICAL_DEVICE_BEGIN_OBJECTS
            LOGICAL_DEVICE_OBJECT(m_Clock)
            LOGICAL_DEVICE_OBJECT(m_Data)
        LOGICAL_DEVICE_END_OBJECTS
    }
    
    LinuxManagementDevice::~LinuxManagementDevice()
    {
    }
    //
    // COSEM Device
    //
    LinuxCOSEMDevice::LinuxCOSEMDevice()
    {
        SERVER_BEGIN_LOGICAL_DEVICES
            SERVER_LOGICAL_DEVICE(m_Management)
        SERVER_END_LOGICAL_DEVICES
    }

    LinuxCOSEMDevice::~LinuxCOSEMDevice()
    {
    }
    //
    // COSEM Engine
    //
    LinuxCOSEMServerEngine::LinuxCOSEMServerEngine(const Options& Opt, Transport * pXPort) :
        COSEMServerEngine(Opt, pXPort)
    {
        ENGINE_BEGIN_DEVICES
            ENGINE_DEVICE(m_Device)
        ENGINE_END_DEVICES    
    }
    
    LinuxCOSEMServerEngine::~LinuxCOSEMServerEngine()
    {
    }
    
    

//            APClient.GetRequest(APPGetRequestOrIndication(SourceAddress,
//                DestinationAddress,
//                5, 
//                COSEMPriority::COSEM_PRIORITY_NORMAL,
//                COSEMServiceClass::COSEM_SERVICE_CONFIRMED,
//                { 1, { 0, 0, 96, 1, 0, 255 }, 2 }));
//
//
//            enum States
//            {
//                ST_OPEN,
//                ST_OPEN_WAIT,
//                ST_OPENED,
//                ST_GET_WAIT,
//                ST_GET_DONE
//            }                           CurrentState = ST_OPEN;
//
//            uint8_t                     SourceAddress = uint8_t(strtol(pSourceAddress, nullptr, 16));
//            COSEMClient                 APClient(SourceAddress);
//            TCPWrapper                  Wrapper(pSocket);
//            COSEMClient::CallbackFunction 
//                                       OpenConfirm = [&](const BaseCallbackParameter& _) -> bool
//            {
//                std::cout << "Association Established\n";
//                return true;
//            };
//            COSEMClient::CallbackFunction 
//                                       GetConfirm = [&](const BaseCallbackParameter& Param) -> bool
//            {
//                std::cout << "Data Retrieved\n";
//                std::cout << dynamic_cast<const APPGetConfirmOrResponse&>(Param).m_Data.ToString() << std::endl;
//                CurrentState = ST_GET_DONE;
//                return true;
//            };
//        
//            std::cout << "Operating as a Client (0x" << hex << uint16_t(SourceAddress) << ")... Opening " << pCOMPortName << "\n";
//            if (SUCCESSFUL != pSocket->Connect(pCOMPortName))
//            {
//                std::cout << "Failed to connect\n";
//                exit(-1);
//            }
//            
//            std::cout << "Connecting to Server (0x" << std::hex << uint16_t(DestinationAddress) << ")\n";
//            std::string Command;
//            bool bProcessed = false;
//            
//            APClient.RegisterTransport(&Wrapper);
//            APClient.RegisterOpenConfirm(OpenConfirm);
//            APClient.RegisterGetConfirm(GetConfirm);
//            
//            bool NeedToProcess = true;
//            while (true)
//            {
//                if (NeedToProcess)
//                {
//                    Wrapper.Process();
//                }
//                switch (CurrentState)
//                {
//                case ST_OPEN:
//                    APClient.OpenRequest(APPOpenRequestOrIndication(SourceAddress, DestinationAddress, 
//                                                                    Security, pPassword == nullptr ? "" : pPassword));
//                    CurrentState = ST_OPEN_WAIT;
//                    break;
//                case ST_OPEN_WAIT:
//                    if (APClient.IsOpen())
//                        CurrentState = ST_OPENED;
//                    break;
//                case ST_OPENED:
//                    APClient.GetRequest(APPGetRequestOrIndication(SourceAddress,
//                        DestinationAddress, 5, 
//                        COSEMPriority::COSEM_PRIORITY_NORMAL,
//                        COSEMServiceClass::COSEM_SERVICE_CONFIRMED,
//                        { 1, { 0, 0, 96, 1, 0, 255 }, 2 }));
//                    CurrentState = ST_GET_WAIT;
//                    break;
//                case ST_GET_WAIT:
//                    break;
//                case ST_GET_DONE:
//                    NeedToProcess = false;
//                    break;
//                }
//            }
    
//    enum States
//    {
//        ST_OPEN,
//        ST_OPEN_WAIT,
//        ST_OPENED,
//        ST_GET_WAIT,
//        ST_GET_DONE
//    }                           CurrentState = ST_OPEN;
//
//    uint8_t                     SourceAddress = uint8_t(strtol(pSourceAddress, nullptr, 16));
//    uint8_t                     DestinationAddress = uint8_t(strtol(pDestinationAddress, nullptr, 16));
//    ISerial::Options::BaudRate  BR = LinuxSerial::Options::BAUD_9600;
//    COSEMClient                 APClient(SourceAddress);
//    HDLCClientLLC               DLClient(HDLCAddress(SourceAddress), 
//        pSerial,
//        HDLCOptions({ StartWithIEC, 3, 500 }));
//    SerialWrapper               DLWrapper(pSerial);
//    COSEMClient::CallbackFunction 
//                               OpenConfirm = [&](const BaseCallbackParameter& _) -> bool
//    {
//        std::cout << "Association Established\n";
//        return true;
//    };
//    COSEMClient::CallbackFunction 
//                               GetConfirm = [&](const BaseCallbackParameter& Param) -> bool
//    {
//        std::cout << "Data Retrieved\n";
//        std::cout << dynamic_cast<const APPGetConfirmOrResponse&>(Param).m_Data.ToString() << std::endl;
//        CurrentState = ST_GET_DONE;
//        return true;
//    };
//        
//    std::cout << "Operating as a Client (0x" << hex << uint16_t(SourceAddress) << ")... Opening " << pCOMPortName << "\n";
//    if (pSerial->Open(pCOMPortName) != SUCCESS ||
//        pSerial->SetOptions(LinuxSerial::Options(BR)) != SUCCESS)
//    {
//        std::cerr << "Error opening port " << pCOMPortName << "\n";
//        exit(-1);
//    }
//
//    std::cout << "Connecting to Server (0x" << std::hex << uint16_t(DestinationAddress) << ")\n";
//    if (IsSerialWrapper)
//    {
//        std::string Command;
//        bool bProcessed = false;
//            
//        APClient.RegisterTransport(&DLWrapper);
//        APClient.RegisterOpenConfirm(OpenConfirm);
//        APClient.RegisterGetConfirm(GetConfirm);
//            
//        bool NeedToProcess = true;
//        while (true)
//        {
//            if (NeedToProcess)
//            {
//                DLWrapper.Process();
//            }
//            switch (CurrentState)
//            {
//            case ST_OPEN:
//                APClient.OpenRequest(APPOpenRequestOrIndication(SourceAddress , DestinationAddress, 
//                    Security, pPassword == nullptr ? "" : pPassword));
//                CurrentState = ST_OPEN_WAIT;
//                break;
//            case ST_OPEN_WAIT:
//                if (APClient.IsOpen())
//                    CurrentState = ST_OPENED;
//                break;
//            case ST_OPENED:
//                APClient.GetRequest(APPGetRequestOrIndication(5, 
//                    COSEMPriority::COSEM_PRIORITY_NORMAL,
//                    COSEMServiceClass::COSEM_SERVICE_CONFIRMED,
//                    { 8, { 0, 0, 1, 0, 0, 255 }, 2 }));
//                CurrentState = ST_GET_WAIT;
//                break;
//            case ST_GET_WAIT:
//                break;
//            case ST_GET_DONE:
//                NeedToProcess = false;
//                break;
//            }
//        }    
    
//    else
//    {
//        bool                        bConfirmation = false;
//        HDLCClientLLC::CallbackFunction 
//                                    ConnectConfirm = [&](const BaseCallbackParameter& _) -> bool
//        {
//            std::cout << "Datalink connection established!\n";
//            bConfirmation = true;
//            return true;
//        };
//            
//        DLClient.RegisterConnectConfirm(ConnectConfirm);
//        APClient.RegisterTransport(&DLClient);
//
//        DLClient.ConnectRequest(DLConnectRequestOrIndication(HDLCAddress(DestinationAddress)));
//        while (DLClient.Process() == RUN_WAIT)
//        {
//            if (bConfirmation)
//            {
//                bConfirmation = false;
//                //
//                // Now we can COSEM open!
//                //
//                APClient.OpenRequest(APPOpenRequestOrIndication(SourceAddress, DestinationAddress));
//            }
//        }    

}